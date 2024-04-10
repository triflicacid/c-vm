#pragma once

#include "Type.hpp"
#include "util.h"
#include <ostream>

namespace language::types {
    class FunctionType : public Type {
    private:
        std::vector<const types::Type *> m_args; // Vector of argument type tokens
        const types::Type *m_ret; // Return type token, may be nullptr
        int m_id;

    public:
        bool is_used_by_fn; // If this type used by a Function object

        FunctionType(lexer::Token *token, int token_pos, const std::vector<const types::Type *>& args, const types::Type *ret)
        : Type(token, token_pos), m_args(args), m_ret(ret), m_id(-1), is_used_by_fn(false) {};

        ~FunctionType() {
            for (auto &arg : m_args) {
                if (!arg->is_single_instance()) delete arg;
            }

            if (m_ret && !m_ret->is_single_instance()) delete m_ret;
        }

        [[nodiscard]] size_t size() const override { return sizeof(WORD_T); };

        /** Check if two signatures are equal */
        [[nodiscard]] bool equal(const FunctionType& other) const;

        /** Get function ID. */
        [[nodiscard]] int id() const { return m_id; }

        /** Is function bound in the program? */
        [[nodiscard]] bool is_stored() const { return m_id > -1; }

        /** Set ID. */
        void set_id(int id) { m_id = id; }

        /** Get argument count. */
        [[nodiscard]] size_t argc() const { return m_args.size(); }

        [[nodiscard]] std::string name() const { return m_token->image(); }

        [[nodiscard]] Category category() const override { return Category::Function; }

        [[nodiscard]] bool is_single_instance() const override { return is_used_by_fn; }

        /** String representation as in definition. */
        [[nodiscard]] std::string repr() const;

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}