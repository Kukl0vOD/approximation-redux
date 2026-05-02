#include "json_builder.h"

namespace json
{
    bool Builder::IsNull() const
    {
        return root_.IsNull();
    }

    bool Builder::IsDict() const
    {
        return !node_stack_.empty() ? node_stack_.top()->IsMap() : false;
    }

    bool Builder::IsArray() const
    {
        return !node_stack_.empty() ? node_stack_.top()->IsArray() : false;
    }

    Dict& Builder::GetDict()
    {
        if (!IsDict())
        {
            throw std::logic_error("Isn't a Dict");
        }
        return std::get<Dict>(node_stack_.top()->GetValue());
    }

    Array& Builder::GetArray()
    {
        if (!IsArray())
        {
            throw std::logic_error("Isn't a Array");
        }
        return std::get<Array>(node_stack_.top()->GetValue());
    }

    void Builder::AddValue(Node node)
    {
        if (IsNull())
        {
            root_ = std::move(node);
        }
        else if (IsDict())
        {
            if (is_key_expected_)
            {
                throw std::logic_error("Key was expected");
            }

            Dict& dict = GetDict();
            dict[current_key_] = std::move(node);
            is_key_expected_ = true;
        }
        else if (IsArray())
        {
            Array& array = GetArray();
            array.push_back(std::move(node));
        }
        else
        {
            throw std::logic_error("Expected to be a Dict, Array, or root node");
        }
    }

    void Builder::AddNode(Node node)
    {
        if (IsNull())
        {
            root_ = std::move(node);
            node_stack_.push(&root_);
        }
        else if (IsDict())
        {
            if (is_key_expected_)
            {
                throw std::logic_error("Key was expected");
            }

            Dict& dict = GetDict();
            dict[current_key_] = std::move(node);
            node_stack_.push(&dict[current_key_]);
        }
        else if (IsArray())
        {
            Array& array = GetArray();
            array.push_back(std::move(node));
            node_stack_.push(&array.back());
        }
        else
        {
            throw std::logic_error("Expected to be a Dict, Array, or root node");
        }
    }

    Builder& Builder::Value(Node value)
    {
        AddValue(std::move(value));
        return *this;
    }

    Builder::DictContext Builder::StartDict()
    {
        AddNode(Dict());
        is_key_expected_ = true;
        return *this;
    }

    Builder::ArrayContext Builder::StartArray()
    {
        AddNode(Array());
        is_key_expected_ = false;
        return *this;
    }

    Builder& Builder::EndDict()
    {
        if (!IsDict())
        {
            throw std::logic_error("End of dict was expected");
        }

        node_stack_.pop();

        if (IsNull())
        {
            is_key_expected_ = false;
        }
        else if (IsArray())
        {
            is_key_expected_ = false;
        }
        else if (IsDict())
        {
            is_key_expected_ = true;
        }

        return *this;
    }

    Builder& Builder::EndArray()
    {
        if (!IsArray())
        {
            throw std::logic_error("End of array was expected");
        }

        node_stack_.pop();

        if (IsNull())
        {
            is_key_expected_ = false;
        }
        else if (IsArray())
        {
            is_key_expected_ = false;
        }
        else if (IsDict())
        {
            is_key_expected_ = true;
        }

        return *this;
    }

    Builder::KeyContext Builder::Key(std::string key)
    {
        if (!is_key_expected_)
        {
            throw std::logic_error("Key wasn't expected");
        }

        current_key_ = std::move(key);
        is_key_expected_ = false;
        return *this;
    }

    Node Builder::Build()
    {
        if (!node_stack_.empty())
        {
            throw std::logic_error("Building incomplete JSON");
        }
        if (root_.IsNull())
        {
            throw std::logic_error("Empty JSON");
        }

        return root_;
    }

    Builder::BaseContext::BaseContext(Builder& builder)
        : builder_(builder)
    {
    }

    Builder::DictContext Builder::BaseContext::StartDict()
    {
        return builder_.StartDict();
    }

    Builder::ArrayContext Builder::BaseContext::StartArray()
    {
        return builder_.StartArray();
    }

    Builder& Builder::BaseContext::EndDict()
    {
        return builder_.EndDict();
    }

    Builder& Builder::BaseContext::EndArray()
    {
        return builder_.EndArray();
    }

    Builder::KeyContext Builder::BaseContext::Key(std::string key)
    {
        return builder_.Key(std::move(key));
    }

    Builder& Builder::BaseContext::Value(Node value)
    {
        builder_.Value(std::move(value));
        return builder_;
    }
}