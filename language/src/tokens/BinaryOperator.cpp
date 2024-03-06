#include "BinaryOperator.h"
#include <map>
#include <iostream>

namespace language::tokens {
    void BinaryOperator::_debug_print(int indent) {
        std::cout << "Binary operator '" << operators::Operator::type_to_string(m_type) << "'" << std::endl;
        if (m_left != nullptr) m_left->debug_print(indent + 1);
        if (m_right != nullptr) m_right->debug_print(indent + 1);
    }
}
