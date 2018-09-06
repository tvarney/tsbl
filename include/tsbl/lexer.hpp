
#pragma once
#ifndef TSBL_LEXER_HPP
#define TSBL_LEXER_HPP

#include <stdint.h>
#include "tsbl/token.hpp"
#include "tsbl/utf8.hpp"

namespace tsbl {
    class Lexer {
    public:
        Lexer();
        ~Lexer();

        void read(utf8::Reader & reader);

        size_t column() const;
        size_t line() const;

        Token next();

        utf8::codepoint_t next_cp();
        utf8::codepoint_t current_cp() const;
        utf8::codepoint_t peek_cp() const;

        bool identifier(utf8::codepoint_t pt) const;
        bool identifier_start(utf8::codepoint_t pt) const;
    private:
        size_t m_CharColumn, m_Line, m_StartLine, m_StartColumn;
        utf8::codepoint_t m_Current, m_Next;
        utf8::Reader * m_Reader;

        Token consume_identifier(const char * start_val);
        Token consume_identifier(utf8::codepoint_t pt);
        void consume_identifier(Token & token);
        Token consume_string(bool dbl, bool longstr);
        Token consume_numeric();
        utf8::codepoint_t consume_escape(size_t hex_digits);

        Token error(utf8::codepoint_t pt) const;
    };
}

#endif
