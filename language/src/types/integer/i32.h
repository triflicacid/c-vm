#pragma once

#include "Integer.h"

namespace language::types::numeric {
    class i32 : public Integer {
    public:
        size_t size_of() override {
            return 4;
        }

        bool is_signed() override {
            return true;
        }

        size_t min_value() override {
            return 0x80000000;
        }

        size_t max_value() override {
            return 0x7FFFFFFF;
        }

        bool can_size_vary() override {
            return false;
        }

        std::string debug_name() override {
            return "i32";
        }
    };
}
