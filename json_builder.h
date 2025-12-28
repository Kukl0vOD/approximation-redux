#pragma once

#include "json.h"

#include <stack>

namespace json
{
    class Builder
    {
    private:
        class               DictContext;
        class               ArrayContext;
        class               KeyContext;
        class               BaseContext;

    public:
        Builder() = default;

        DictContext         StartDict();
        ArrayContext        StartArray();
        Builder&            EndDict();
        Builder&            EndArray();
        KeyContext          Key(std::string key);
        Builder&            Value(Node value);

        Node                Build();

    private:
        Node                root_;
        std::stack<Node*>   node_stack_;
        std::string         current_key_;
        bool                is_key_expected_ = false;

        bool                IsNull() const;
        bool                IsDict() const;
        bool                IsArray() const;

        Dict&               GetDict();
        Array&              GetArray();

        void                AddValue(Node node);
        void                AddNode(Node node);
    };

    class Builder::BaseContext
    {
    public:
        BaseContext(Builder& builder);

        DictContext         StartDict();
        ArrayContext        StartArray();
        Builder&            EndDict();
        Builder&            EndArray();
        KeyContext          Key(std::string key);
        Builder&            Value(Node value);

    protected:
        Builder& builder_;
    };

    class Builder::DictContext : public BaseContext
    {
    public:
        using BaseContext::BaseContext;

        Builder&            Value(Node) = delete;
        DictContext         StartDict() = delete;
        ArrayContext        StartArray() = delete;
        Builder&            EndArray() = delete;
        Node                Build() = delete;
    };

    class Builder::ArrayContext : public BaseContext
    {
    public:
        using BaseContext::BaseContext;

        KeyContext          Key(std::string) = delete;
        Builder&            EndDict() = delete;
        Node                Build() = delete;
    };

    class Builder::KeyContext : public BaseContext
    {
    public:
        using BaseContext::BaseContext;

        KeyContext          Key(std::string) = delete;
        Builder&            EndDict() = delete;
        Builder&            EndArray() = delete;
        Node                Build() = delete;
    };
}