#include <iostream>
#include "Token.h"
#include "SymbolDecl.h"
#include "parser/Data.h"
#include <string>

namespace language::tokens {
    void SymbolDeclaration::_debug_print(int indent) {
        std::cout << "Declare symbol " << m_symbol << " of type " << m_type->debug_name() << " (sizeof=" << m_type->size_of() << ")" << std::endl;
    }
}
