#pragma once

#include <string>
#include <map>

namespace language::operators {
    class Operator {
    public:
        enum Type {
            addition,
            subtraction,
            multiplication,
            assignment
        };

        static const std::map<Type, int> precedence;

        [[nodiscard]] static std::string type_to_string(Type &type);
    };
}
