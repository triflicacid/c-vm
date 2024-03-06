#pragma once

#include "types/NumericType.h"

namespace language::types::numeric {
    class Integer : public NumericType {
    public:
        [[nodiscard]] bool is_integral() override {
            return true;
        }

        /** Is this type able to represent negative values? */
        [[nodiscard]] virtual bool is_signed() { return true; }

        /** Return magnitude of minimum value */
        [[nodiscard]] virtual size_t min_value() { return 0; }

        /** Return maximum value */
        [[nodiscard]] virtual size_t max_value() { return 0; }

        /** Check if the given value will fit in the provided integer */
        [[nodiscard]] static bool does_fit(unsigned long long value, bool is_neg, Integer *integer);
    };
}
