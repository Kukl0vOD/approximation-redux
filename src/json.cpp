#include "json.h"

#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

namespace json {

    namespace loader {

        Node LoadNode(istream& input);

        void SkipWhitespace(istream& input) {
            while (input.peek() == ' ' || input.peek() == '\t' || input.peek() == '\r' || input.peek() == '\n') {
                input.get();
            }
        }

        bool IsValueEnd(char c) {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n' ||
                c == ',' || c == ']' || c == '}' || c == EOF;
        }

        Node LoadNull(istream& input) {
            string word;
            for (int i = 0; i < 4; ++i) {
                word += static_cast<char>(input.get());
            }
            if (word != "null" || !IsValueEnd(input.peek())) {
                throw ParsingError("Invalid null value");
            }
            return Node(nullptr);
        }

        Node LoadBool(istream& input) {
            string word;
            char c = input.get();

            if (c == 't') {
                word = "t";
                for (int i = 0; i < 3; ++i) {
                    word += static_cast<char>(input.get());
                }
                if (word != "true" || !IsValueEnd(input.peek())) {
                    throw ParsingError("Invalid boolean value");
                }
                return Node(true);
            }
            else if (c == 'f') {
                word = "f";
                for (int i = 0; i < 4; ++i) {
                    word += static_cast<char>(input.get());
                }
                if (word != "false" || !IsValueEnd(input.peek())) {
                    throw ParsingError("Invalid boolean value");
                }
                return Node(false);
            }
            throw ParsingError("Invalid boolean value");
        }

        Node LoadNumber(istream& input) {
            string number_str;
            bool is_double = false;

            if (input.peek() == '-') {
                number_str += static_cast<char>(input.get());
            }

            while (isdigit(input.peek())) {
                number_str += static_cast<char>(input.get());
            }

            if (input.peek() == '.') {
                is_double = true;
                number_str += static_cast<char>(input.get());
                while (isdigit(input.peek())) {
                    number_str += static_cast<char>(input.get());
                }
            }

            if (input.peek() == 'e' || input.peek() == 'E') {
                is_double = true;
                number_str += static_cast<char>(input.get());
                if (input.peek() == '+' || input.peek() == '-') {
                    number_str += static_cast<char>(input.get());
                }
                while (isdigit(input.peek())) {
                    number_str += static_cast<char>(input.get());
                }
            }

            if (number_str.empty()) {
                throw ParsingError("Invalid number");
            }

            if (is_double) {
                try {
                    return Node(stod(number_str));
                }
                catch (...) {
                    throw ParsingError("Invalid double number");
                }
            }
            else {
                try {
                    return Node(stoi(number_str));
                }
                catch (...) {
                    throw ParsingError("Invalid integer number");
                }
            }
        }

        string LoadEscapeSequence(istream& input) {
            char c = input.get();

            switch (c) {
            case 'n':
                return "\n";
            case 'r':
                return "\r";
            case 't':
                return "\t";
            case '"':
                return "\"";
            case '\\':
                return "\\";
            default:
                throw ParsingError("Invalid escape sequence");
            }
        }

        Node LoadString(istream& input) {
            string result;
            while (true) {
                char c = input.get();
                if (c == '"') {
                    break;
                }
                else if (c == '\\') {
                    result += LoadEscapeSequence(input);
                }
                else if (c == '\n' || c == '\r') {
                    throw ParsingError("Unescaped newline in string");
                }
                else if (c == EOF) {
                    throw ParsingError("Unexpected end of string");
                }
                else {
                    result += c;
                }
            }
            return Node(move(result));
        }

        Node LoadArray(istream& input) {
            Array result;

            SkipWhitespace(input);
            if (input.peek() == ']') {
                input.get();
                return Node(move(result));
            }

            while (true) {
                SkipWhitespace(input);
                result.push_back(LoadNode(input));
                SkipWhitespace(input);

                char c = input.get();
                if (c == ']') {
                    break;
                }
                else if (c != ',') {
                    throw ParsingError("Expected ',' or ']' in array");
                }
            }

            return Node(move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;

            SkipWhitespace(input);
            if (input.peek() == '}') {
                input.get();
                return Node(move(result));
            }

            while (true) {
                SkipWhitespace(input);

                if (input.get() != '"') {
                    throw ParsingError("Expected '\"' as dictionary key");
                }
                string key = LoadString(input).AsString();

                SkipWhitespace(input);

                if (input.get() != ':') {
                    throw ParsingError("Expected ':' after dictionary key");
                }

                SkipWhitespace(input);

                result.insert({ move(key), LoadNode(input) });

                SkipWhitespace(input);

                char c = input.get();
                if (c == '}') {
                    break;
                }
                else if (c != ',') {
                    throw ParsingError("Expected ',' or '}' in dictionary");
                }
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            SkipWhitespace(input);
            char c = input.peek();

            if (c == '[') {
                input.get();
                return LoadArray(input);
            }
            else if (c == '{') {
                input.get();
                return LoadDict(input);
            }
            else if (c == '"') {
                input.get();
                return LoadString(input);
            }
            else if (c == 'n') {
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                return LoadBool(input);
            }
            else if (c == '-' || isdigit(c)) {
                return LoadNumber(input);
            }
            else {
                throw ParsingError("Unexpected character");
            }
        }

        Node LoadNode(ifstream& input) {
            SkipWhitespace(input);
            char c = input.peek();

            if (c == '[') {
                input.get();
                return LoadArray(input);
            }
            else if (c == '{') {
                input.get();
                return LoadDict(input);
            }
            else if (c == '"') {
                input.get();
                return LoadString(input);
            }
            else if (c == 'n') {
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                return LoadBool(input);
            }
            else if (c == '-' || isdigit(c)) {
                return LoadNumber(input);
            }
            else {
                throw ParsingError("Unexpected character");
            }
        }

    }  // namespace

    Node::Node()
        : value_(nullptr)
    {
    }

    Node::Node(const Value& other)
        : value_(other)
    {
    }

    Node::Node(std::nullptr_t)
        : value_(nullptr)
    {
    }

    Node::Node(const Array& array)
        : value_(array) {
    }

    Node::Node(const Dict& map)
        : value_(map) {
    }

    Node::Node(int value)
        : value_(value) {
    }

    Node::Node(bool value)
        : value_(value)
    {
    }

    Node::Node(double value)
        : value_(value)
    {
    }

    Node::Node(const string& value)
        : value_(value)
    {
    }

    Node::Node(std::string_view value)
        : value_(std::string(value))
    {
    }

    const Value& Node::GetValue() const
    {
        return value_;
    }

    Value& Node::GetValue()
    {
        return value_;
    }

    TypeList Node::GetType() const
    {
        if (IsInt())
        {
            return TypeList::INT;
        }
        if (IsPureDouble())
        {
            return TypeList::DOUBLE;
        }
        if (IsBool())
        {
            return TypeList::BOOL;
        }
        if (IsString())
        {
            return TypeList::STRING;
        }
        if (IsArray())
        {
            return TypeList::ARRAY;
        }
        if (IsMap())
        {
            return TypeList::DICT;
        }

        return TypeList::NULLPTR;
    }

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const
    {
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(value_);
    }

    int Node::AsInt() const
    {
        if (IsInt())
        {
            return std::get<int>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    double Node::AsDouble() const
    {
        if (IsInt())
        {
            return static_cast<double>(std::get<int>(value_));
        }
        else if (IsPureDouble())
        {
            return std::get<double>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    bool Node::AsBool() const
    {
        if (IsBool())
        {
            return std::get<bool>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    const std::string& Node::AsString() const
    {
        if (IsString())
        {
            return std::get<std::string>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    const Array& Node::AsArray() const
    {
        if (IsArray())
        {
            return std::get<Array>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    const Dict& Node::AsMap() const
    {
        if (IsMap())
        {
            return std::get<Dict>(value_);
        }
        else
        {
            throw std::logic_error("type mismatch");
        }
    }

    bool Node::operator==(const Node& other) const
    {
        TypeList left = GetType();
        TypeList right = other.GetType();

        if (left == right)
        {
            switch (left)
            {
            case TypeList::INT:
                return AsInt() == other.AsInt();
            case TypeList::DOUBLE:
                return AsDouble() == other.AsDouble();
            case TypeList::STRING:
                return AsString() == other.AsString();
            case TypeList::BOOL:
                return AsBool() == other.AsBool();
            case TypeList::ARRAY:
                return AsArray() == other.AsArray();
            case TypeList::DICT:
                return AsMap() == other.AsMap();
            case TypeList::NULLPTR:
                return true;
            default:
                return false;
            }
        }

        return false;
    }

    bool Node::operator!=(const Node& other) const
    {
        return !(*this == other);
    }


    Document Load(std::istream& input)
    {
        return Document(loader::LoadNode(input));
    }

    void Print(const Document& doc, std::ostream& output) {
        Printer printer;
        PrintContext ctx{ output };
        printer.PrintValue(doc.GetRoot().GetValue(), ctx);
    }

    Document::Document(Node root)
        : root_(root)
    {
    }

    bool Document::operator==(const Document& other) const
    {
        return root_ == other.root_;
    }

    bool Document::operator!=(const Document& other) const
    {
        return !(*this == other);
    }

    const Node& Document::GetRoot() const
    {
        return root_;
    }
}