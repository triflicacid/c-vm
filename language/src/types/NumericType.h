#pragma once

#include "Type.h"
#include <string>

namespace language::types {
    class NumericType : public Type {
    public:
        [[nodiscard]] bool is_numeric() override {
            return true;
        }

        /** Are we an integer or a float? */
        [[nodiscard]] virtual bool is_integral() { return false; }
    };
}
