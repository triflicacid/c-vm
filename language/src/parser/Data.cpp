#include <string>
#include "Data.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include "tokens/SymbolDecl.h"
#include "util.h"

namespace language::parser {
    int SymbolScope::get_offset(const std::string &symbol) {
        auto it = symbols.find(symbol);
        if (it == symbols.end()) {
            return -1;
        } else {
            return it->second.second;
        }
    }

    tokens::SymbolDeclaration *SymbolScope::get_decl(const std::string &symbol) {
        auto it = symbols.find(symbol);
        if (it == symbols.end()) {
            return nullptr;
        } else {
            return it->second.first;
        }
    }

    int SymbolScope::insert_symbol(const std::string& symbol, tokens::SymbolDeclaration *declaration) {
        auto it = symbols.find(symbol);
        if (it == symbols.end()) {
            symbols.insert({ symbol, {declaration, scope_offset} });
        } else {
            it->second.first = declaration;
            it->second.second = scope_offset;
        }

        int size = (int) declaration->get_symbol_type()->size_of();
        scope_offset += size;
        return scope_offset - size;
    }

    int SourceData::push_symbol_scope() {
        m_symbol_scopes.emplace_back();
        return (int) m_symbol_scopes.size() - 1;
    }

    void SourceData::pop_symbol_scope() {
        if (m_symbol_scopes.empty()) return;
        m_symbol_scopes.pop_back();
    }

    int SourceData::get_symbol_scope(const std::string &symbol) {
        for (int i = (int) m_symbol_scopes.size() - 1; i >= 0; i--) {
            if (m_symbol_scopes[i].get_offset(symbol) != -1) {
                return i;
            }
        }

        return -1;
    }

    int SourceData::get_symbol_offset(const std::string &symbol) {
        for (int i = (int) m_symbol_scopes.size() - 1; i >= 0; i--) {
            int off = m_symbol_scopes[i].get_offset(symbol);

            if (off != -1) {
                return off;
            }
        }

        return -1;
    }

    tokens::SymbolDeclaration *SourceData::get_symbol_declaration(const std::string &symbol) {
        for (int i = (int) m_symbol_scopes.size() - 1; i >= 0; i--) {
            auto declaration = m_symbol_scopes[i].get_decl(symbol);

            if (declaration != nullptr) {
                return declaration;
            }
        }

        return nullptr;
    }

    void SourceData::insert_symbol(const std::string& symbol, tokens::SymbolDeclaration *declaration) {
        if (m_symbol_scopes.empty()) push_symbol_scope();
        m_symbol_scopes.back().insert_symbol(symbol, declaration);
    }

    void SourceData::set_contents(std::string contents) {
        m_contents = std::move(contents);
        reset();
    }

    void SourceData::reset() {
        m_parsed = false;
        reset_tokens();
        m_symbol_scopes.clear();
    }

    void SourceData::reset_tokens() {
        for (auto token : m_tokens) {
            delete token;
        }

        m_tokens.clear();
    }

    bool SourceData::load_from_file() {
        std::ifstream file;
        file.open(m_path);

        if (!file.is_open()) {
            return false;
        }

        std::string line;
        std::stringstream stream;
        while (std::getline(file, line)) {
            stream << line << '\n';
        }

        set_contents(stream.str());

        return true;
    }

    void Data::add_message(Message::Level level, std::string msg, int idx, int len) {
        add_message(m_source, level, std::move(msg), idx, len);
    }

    void Data::add_message(const std::string &source_name, Message::Level level, std::string msg, int idx, int len) {
        SourceData *source_data = get_source(source_name);
        std::string src = source_data->get_contents();
        std::string err_src = extract_substr_between_newlines(source_data->get_contents(), idx, len);

        Message message(level, m_row, idx, idx, len, source_data->get_path(), std::move(msg), std::move(err_src));
        m_messages.push_back(message);
    }

    void Data::build() {
        m_out_stream.clear();
        for (auto &pair : m_sources) {
            pair.second.build();
            m_out_stream << pair.second.out();
        }
    }
}
