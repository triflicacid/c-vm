#pragma once

#include <string>

namespace language::types {
    class Type {
    public:
        /** Size of the type in bytes */
        [[nodiscard]] virtual size_t size_of() {
            return 0;
        }

        [[nodiscard]] virtual bool is_numeric() {
            return false;
        }

        /** Return: is this type vary-sized? */
        [[nodiscard]] virtual bool can_size_vary() { return true; }

        /** Return string representation of type name */
        [[nodiscard]] virtual std::string debug_name() {
            return "Type::base";
        }
    };
}
