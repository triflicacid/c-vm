#pragma once

#include "processor/src/opcodes.h"
#include "argument.hpp"

#include <string>
#include <vector>

namespace assembler::instruction {
    enum class ParamType {
        Literal,
        Address,
        Register,
        RegisterPointer
    };

    struct Param {
        ParamType type;
        int size;
    };

    class Signature {
    private:
        std::string m_mnemonic;
        std::vector<Param> m_params;
        std::vector<int> m_sizes;
        OPCODE_T m_opcode;

    public:
        Signature(std::string mnemonic, std::vector<Param> params, OPCODE_T opcode);

        [[nodiscard]] std::string get_mnemonic() const { return m_mnemonic; }

        size_t param_count() { return m_params.size(); }

        Param *get_param(int i) { return &m_params[i]; }

        [[nodiscard]] OPCODE_T get_opcode() const { return m_opcode; }

        /** Get byte-size of entire instruction. */
        int get_bytes();

        /** Return whether the given opcode exists. */
        static bool exists(const std::string& mnemonic);

        /** Find the signature given mnemonic and arguments. */
        static Signature *find(const std::string& mnemonic, const std::vector<ArgumentType>& args);
    };

    /** Return whether the argument matches against the parameter. */
    bool is_match(ArgumentType arg, ParamType param);
}
