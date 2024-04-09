#pragma once

#include <map>

#include "Symbol.hpp"
#include "types/UserType.hpp"

namespace language::parser {
    /** Represents a scope instance. If a symbol's offset>0, means it exists physically on the stack. */
    class Scope {
    private:
        size_t m_offset;
        size_t m_size;
        std::map<std::string, std::pair<const Symbol *, int>> m_symbols; // symbol name => (symbol, offset)
        std::map<std::string, const types::UserType *> m_user_types;

        void grow(size_t n) {
            m_offset += n;
            m_size += n;
        }

    public:
        Scope() : m_offset(0), m_size(0) {};

        ~Scope() {
            for (auto& pair : m_symbols) {
                delete pair.second.first;
            }
        }

        /** May be useful when this is a "mock" scope and does not represent an actual stack frame boundary. */
        explicit Scope(size_t offset) : m_offset(offset), m_size(0) {};

        void clear();

        [[nodiscard]] size_t size() const { return m_size; }

        /** Query if symbol exists. */
        bool exists(const std::string& name);

        /** Get symbol information. */
        const std::pair<const Symbol *, int> *get(const std::string& name);

        /** Insert new symbol. Note, this overwrites the old instance. Return reference to this old reference. */
        const Symbol *create(const Symbol *symbol, bool has_size = true);

        /** Query if data type exists. */
        bool data_exists(const std::string& name);

        /** Get symbol information. */
        const types::UserType *data_get(const std::string& name);

        /** Create new data-type. */
        void data_create(const types::UserType *data);

        /** DEBUG: print symbols to stream */
        void debug_print(std::ostream& stream, const std::string& prefix = "");
    };
}
