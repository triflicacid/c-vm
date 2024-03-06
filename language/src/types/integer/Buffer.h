#pragma once

#include "Integer.h"

namespace language::types::numeric {
    // TODO
    class Buffer : public Integer {
    private:
        size_t m_size;

    public:
        explicit Buffer(size_t size) : m_size(size) {};

        size_t size_of() override {
            return m_size;
        }

        bool is_signed() override {
            return false;
        }

        size_t min_value() override {
            return 0;
        }

        size_t max_value() override {
            return 0;
        }

        bool can_size_vary() override {
            return true;
        }
    };
}
