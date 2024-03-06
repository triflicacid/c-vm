#pragma once

#include "Integer.h"

namespace language::types::numeric {
    class u64 : public Integer {
    public:
        size_t size_of() override {
            return 8;
        }

        bool is_signed() override {
            return false;
        }

        size_t min_value() override {
            return 0;
        }

        size_t max_value() override {
            return 0xFFFFFFFFFFFFFFFF;
        }

        bool can_size_vary() override {
            return false;
        }

        std::string debug_name() override {
            return "u64";
        }
    };
}
