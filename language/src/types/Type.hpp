#pragma once

#include "lexer/Token.hpp"

namespace language::types {
    enum class Category {
        None,
        Numeric,
        User,
        Function,
    };

    class Type {
    public:
        [[nodiscard]] virtual size_t size() const { return 0; }

        [[nodiscard]] virtual Category category() const { return Category::None; }

        /** Return if there should only be a single instance of this specific type. Used to know if we should delete it. */
        [[nodiscard]] virtual bool is_single_instance() const { return false; }

        [[nodiscard]] virtual std::string repr() const { return ""; }

        virtual void debug_print(std::ostream& stream, const std::string& prefix) const {};
    };
}
