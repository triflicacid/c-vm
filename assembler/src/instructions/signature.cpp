#include "signature.hpp"
#include "signatures.hpp"
#include "argument.hpp"

#include <algorithm>

namespace assembler::instruction {
    Signature::Signature(std::string mnemonic, std::vector<Param> params, OPCODE_T opcode) {
        m_mnemonic = std::move(mnemonic);
        m_params = std::move(params);
        m_opcode = opcode;
    }

    bool Signature::exists(const std::string &mnemonic) {
        return std::any_of(signature_list.begin(), signature_list.end(), [&](const auto &signature) {
            return signature.m_mnemonic == mnemonic;
        });
    }

    Signature *Signature::find(const std::string& mnemonic, const std::vector<ArgumentType>& args) {
        for (auto &signature : signature_list) {
            if (signature.m_mnemonic == mnemonic && signature.m_params.size() == args.size()) {
                bool match = true;

                for (int i = 0; i < args.size(); ++i) {
                    if (!is_match(args[i], signature.get_param(i)->type)) {
                        match = false;
                        break;
                    }
                }

                if (match)
                    return &signature;
            }
        }

        return nullptr;
    }

    int Signature::get_bytes() {
        int size = (int) sizeof(m_opcode);

        for (const Param& param : m_params) {
            size += param.size;
        }

        return size;
    }

    bool is_match(ArgumentType arg, ParamType param) {
        switch (param) {
            case ParamType::Literal:
                return arg == ArgumentType::Literal || arg == ArgumentType::LabelLiteral;
            case ParamType::Address:
                return arg == ArgumentType::Address || arg == ArgumentType::LabelAddress;
            case ParamType::RegisterPointer:
                return arg == ArgumentType::RegisterPointer;
            case ParamType::Register:
                return arg == ArgumentType::Register;
            default:
                return false;
        }
    }
}
