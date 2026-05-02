#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json
{
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    enum class TypeList
    {
        INT,
        DOUBLE,
        STRING,
        BOOL,
        ARRAY,
        DICT,
        NULLPTR
    };

    class Node {
    public:
        Node();
        Node(const Value& other);
        Node(std::nullptr_t);
        Node(const Array& array);
        Node(const Dict& map);
        Node(int value);
        Node(bool value);
        Node(double value);
        Node(const std::string& value);
        Node(std::string_view value);

        const Value&        GetValue() const;
        Value&              GetValue();
        TypeList            GetType() const;

        bool                IsInt() const;
        bool                IsDouble() const;
        bool                IsPureDouble() const;
        bool                IsBool() const;
        bool                IsString() const;
        bool                IsNull() const;
        bool                IsArray() const;
        bool                IsMap() const;

        int                 AsInt() const;
        double              AsDouble() const;
        bool                AsBool() const;
        const std::string&  AsString() const;
        const Array&        AsArray() const;
        const Dict&         AsMap() const;

        bool                operator==(const Node& other) const;
        bool                operator!=(const Node& other) const;

    private:
        Value               value_;
    };

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    struct PrintVisitor {
        PrintContext ctx;

        void operator()(std::nullptr_t) {
            ctx.out << "null";
        }

        void operator()(const Array& array) {
            ctx.out << "[";
            bool first = true;

            for (const auto& item : array) {
                if (!first) {
                    ctx.out << ", ";
                }
                first = false;
                PrintVisitor visitor{ ctx };
                std::visit(visitor, item.GetValue());
            }

            ctx.out << "]";
        }

        void operator()(const Dict& dict) {
            ctx.out << "{";
            bool first = true;

            for (const auto& [key, value] : dict) {
                if (!first) {
                    ctx.out << ", ";
                }

                first = false;

                ctx.out << "\"";

                for (char c : key) {
                    switch (c) {
                    case '\"': 
                        ctx.out << "\\\"";
                        break;
                    case '\\':
                        ctx.out << "\\\\";
                        break;
                    case '\n':
                        ctx.out << "\\n";
                        break;
                    case '\r':
                        ctx.out << "\\r";
                        break;
                    case '\t':
                        ctx.out << "\\t";
                        break;
                    default:
                        ctx.out << c;
                        break;
                    }
                }

                ctx.out << "\": ";
                PrintVisitor visitor{ ctx };
                std::visit(visitor, value.GetValue());
            }

            ctx.out << "}";
        }

        void operator()(bool value) {
            ctx.out << (value ? "true" : "false");
        }

        void operator()(int value) {
            ctx.out << value;
        }

        void operator()(double value) {
            ctx.out << value;
        }

        void operator()(const std::string& value) {
            ctx.out << "\"";

            for (char c : value) {
                switch (c) {
                case '\"': 
                    ctx.out << "\\\"";
                    break;
                case '\\': 
                    ctx.out << "\\\\";
                    break;
                case '\n': 
                    ctx.out << "\\n";
                    break;
                case '\r': 
                    ctx.out << "\\r"; 
                    break;
                case '\t': 
                    ctx.out << "\\t"; 
                    break;
                default: 
                    ctx.out << c; 
                    break;
                }
            }
            ctx.out << "\"";
        }
    };

    struct Printer {
        void PrintValue(const Value& value, const PrintContext& context) {
            PrintVisitor visitor{ context };
            std::visit(visitor, value);
        }
    };

    class Document {
    public:
        explicit            Document(Node root);

        const Node&         GetRoot() const;

        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

    private:
        Node                root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
}