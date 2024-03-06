#include "Operator.h"
#include <map>

namespace language::operators {
    const std::map<Operator::Type, int> Operator::precedence = {
            { Type::addition, 3 },
            { Type::subtraction, 3 },
            { Type::multiplication, 2 },
            { Type::assignment, 5 }
    };

    std::string Operator::type_to_string(Operator::Type &type) {
        switch (type) {
            case Type::addition:
                return "+";
            case Type::subtraction:
                return "-";
            case Type::multiplication:
                return "*";
            case Type::assignment:
                return "=";
        }
    }
}
