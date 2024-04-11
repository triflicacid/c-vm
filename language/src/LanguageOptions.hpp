#pragma once

namespace language::options {
    extern char default_entry_point[];
    extern bool allow_shadowing;  // Allow variable shadowing
    extern bool must_declare_functions;  // Functions must be declared before they are defined
    extern int unused_symbol_level;  // Reporting level of "unused symbol", -1 is silent
}
