
#pragma once
#ifndef TSBL_LEXER_HPP
#define TSBL_LEXER_HPP

#include <stdint.h>
#include <string>
#include "tsbl/utf8.hpp"

namespace tsbl {
    class Token {
    public:
        enum Id : int32_t {
            NewLine,       //< (\n(\r)?)|\r

            // Operators
            Plus,          //< +
            Minus,         //< -
            Divide,        //< /
            Multiply,      //< *
            Power,         //< **
            OpenParen,     //< (
            CloseParen,    //< )
            OpenBracket,   //< [
            CloseBracket,  //< ]
            OpenBrace,     //< {
            CloseBrace,    //< }
            Access,        //< .
            Assign,        //< =
            Not,           //< !
            Equals,        //< ==
            NotEquals,     //< !=
            Greater,       //< >
            GreaterEquals, //< >=
            Less,          //< <
            LessEquals,    //< <=
            RShift,        //< >>
            LShift,        //< <<

            // Keywords
            //---------
            // Values
            True,          //< true
            False,         //< false
            Null,          //< null

            // Classes & Methods
            Struct,        //< struct
            Class,         //< class
            Public,        //< public
            Private,       //< private
            Protected,     //< protected
            Super,         //< super
            Def,           //< def
            Return,        //< return

            // Loops & Conditionals
            For,           //< for
            While,         //< while
            If,            //< if
            Else,          //< else
            Elif,          //< elif
            Switch,        //< switch
            Break,         //< break
            Continue,      //< continue

            // Exceptions
            Try,           //< try
            Catch,         //< catch
            Throw,         //< throw

            // lvalues & rvalues
            Integer,       //< [0-9]+
            Float,         //< [0-9]+\.[0-9]*([eE][0-9]+)?
            String,        //< ("")|('')
            LongString,    //< (""" """)|(''' ''')
            Identifier,    //< [_a-zA-Z][_a-zA-Z0-9]*  <-- Use character categories

            _COUNT,        //< Used for bounds checking - not a token

            // Errors - EOF, Bad Encoding, etc
            EndOfFile = -1,
            BadEncoding = -2,
            MissingEndQuotes = -3,
            UnknownSymbol = -4
        };

        static const char * Name(Token::Id id);

        typedef std::basic_string<utf8::codepoint_t> Utf8String;
    public:
        Token(Token::Id id, size_t line_no, size_t column);
        Token(const Token & source);
        Token(Token && source);
        ~Token();

        Token & operator=(const Token & source);
        Token & operator=(Token && source);

        void make_value();
        void make_value(const Token::Utf8String & value);
        void make_value(Token::Utf8String && value);
        bool has_value() const;
        Token::Utf8String * take_value();
        Token::Utf8String & value();
        const Token::Utf8String & value() const;

        Token::Id id() const;
        size_t line() const;
        size_t column() const;
        const char * name() const;
    private:
        size_t m_Line, m_Column;
        Token::Id m_Id;
        Utf8String * m_StringData;
    };

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
        size_t m_CharColumn, m_Line;
        utf8::codepoint_t m_Current, m_Next;
        utf8::Reader * m_Reader;

        Token consume_identifier(const char * start_val);
        Token consume_identifier(utf8::codepoint_t pt);
        void consume_identifier();
        Token consume_string(bool dbl, bool longstr);
        Token consume_numeric();
    };
}

#endif
