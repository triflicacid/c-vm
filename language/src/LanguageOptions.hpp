#pragma once

namespace language {
    struct LanguageOptions {
        bool allow_shadowing = true;  // Allow variable shadowing
        bool must_declare_functions = false;  // Functions must be declared before they are defined
    };
}
