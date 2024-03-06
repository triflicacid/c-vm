#pragma once

namespace language::parser {
    struct Options {
        bool allow_shadowing = true; // Allow inner scope variable to "shadow" an outer scope variable
        bool allow_symbol_redeclaration = true; // Allow redefinition of a symbol in the same scope
    };
}
