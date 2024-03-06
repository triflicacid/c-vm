#include "Integer.h"

namespace language::types::numeric {
    bool Integer::does_fit(unsigned long long value, bool is_neg, Integer *integer) {
        if (value > integer->max_value()) {
            return false;
        }

        // Check that signs match
        if ((is_neg && integer->min_value() >= 0) || (!is_neg && integer->min_value() < 0)) {
            return false;
        }

        if (is_neg && value > -integer->min_value()) {
            return false;
        }

        return true;
    }
}
