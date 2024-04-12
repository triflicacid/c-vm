#pragma once

#include "Type.hpp"
#include <ostream>

namespace language::types {
    class UnitType : public Type {
    public:
        [[nodiscard]] std::string repr() const override { return "()"; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<UnitType />";
        };
    };
}
