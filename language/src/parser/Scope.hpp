#pragma once

#include "language/src/statement/Symbol.hpp"
#include "types/UserType.hpp"
#include "types/FunctionType.hpp"

namespace language::parser {
    class Scope {
    private:
        size_t m_offset;
        size_t m_size;
        std::map<std::string, parser::SymbolDeclaration *> m_symbols; // Physical stack
        std::map<std::string, const types::UserType *> m_user_types;
        std::map<std::string, std::vector<const types::FunctionType *>> m_functions;
        int m_func_id; // ID of invoker function

        void grow(size_t n) {
            m_offset += n;
            m_size += n;
        }

    public:
        explicit Scope(int func_id) : m_offset(0), m_size(0), m_func_id(func_id) {};

        ~Scope() {
            for (auto& pair : m_symbols) {
                delete pair.second;
            }
        }

        /** May be useful when this is a "mock" scope and does not represent an actual stack frame boundary. */
        Scope(int func_id, size_t offset) : m_offset(offset), m_size(0), m_func_id(func_id) {};

        void clear();

        [[nodiscard]] size_t size() const { return m_size; }

        /** Return invoker function ID. */
        [[nodiscard]] int invoker_id() const { return m_func_id; }

        /** Query if symbol var_exists. */
        bool var_exists(const std::string& name);

        /** Get symbol information. */
        parser::SymbolDeclaration *var_get(const std::string& name);

        /** Insert new symbol. Note, this overwrites the old instance. Return reference to this old reference. Updates symbol's offset. */
        parser::SymbolDeclaration *var_create(parser::SymbolDeclaration *symbol);

        /** Query if data type var_exists. */
        bool data_exists(const std::string& name);

        /** Get symbol information. */
        const types::UserType *data_get(const std::string& name);

        /** Create new data-type. */
        void data_create(const types::UserType *data);

        /** Query if function name var_exists. */
        bool func_exists(const std::string& name);

        /** Query if function overload name var_exists. */
        bool func_exists(const std::string& name, const types::FunctionType *overload);

        /** Get function. */
        const std::vector<const types::FunctionType *> *func_get(const std::string& name);

        /** Get function overload which matches the inputs. */
        const types::FunctionType *func_get(const std::string& name, const types::FunctionType *overload) const;

        /** Create new function. */
        void func_create(const std::string& name, const types::FunctionType *overload);

        [[nodiscard]] std::vector<const parser::SymbolDeclaration *> symbols() const;

        /** DEBUG: print symbols to stream */
        void debug_print(std::ostream& stream, const std::string& prefix = "");
    };
}
