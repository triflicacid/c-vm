#pragma once

#include "parser/Scope.hpp"
#include "Source.hpp"
#include "statement/Function.hpp"

namespace language {
    class Program {
    private:
        int func_id;
        Source *m_source;
        parser::Scope *m_scope; // Global scope
        std::map<int, statement::Function *> m_funcs;

    public:
        explicit Program(Source *source) : m_source(source), func_id(0) {
            m_scope = new parser::Scope(-1); // Fake invoker ID - never used in topmost scope.
        }

        ~Program() {
            delete m_scope;

            for (auto& pair : m_funcs) {
                delete pair.second;
            }
        }

        [[nodiscard]] Source *source() const { return m_source; }

        [[nodiscard]] parser::Scope *global_scope() const { return m_scope; }

        /** Return a new function ID, increment. */
        int new_function_id() { return func_id++; }

        void register_function(statement::Function *func) {
            m_funcs.insert({ func->id(), func });
        }

        [[nodiscard]] statement::Function *get_function(int id) const {
            auto pair = m_funcs.find(id);
            return pair == m_funcs.end() ? nullptr : pair->second;
        }

        void debug_print(std::ostream& stream, const std::string& prefix = "");
    };
}
