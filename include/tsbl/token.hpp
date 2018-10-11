
#pragma once
#ifndef TSBL_TOKEN_HPP
#define TSBL_TOKEN_HPP

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
            Increment,     //< ++
            Minus,         //< -
            Decrement,     //< --
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

            // Types
            Int8,          //< int8
            Int16,         //< int16
            Int32,         //< int32
            Int64,         //< int64
            UInt8,         //< uint8
            UInt16,        //< uint16
            UInt32,        //< uint32
            UInt64,        //< uint64
            Float,         //< float
            Double,        //< double
            Char,          //< char
            String,        //< string

            // Classes & Methods
            Struct,        //< struct
            Class,         //< class
            Public,        //< public
            Private,       //< private
            Protected,     //< protected
            Super,         //< super
            Def,           //< def
            Return,        //< return
            Pure,          //< pure

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
            IntegerValue,  //< [0-9]+
            RealValue,     //< [0-9]+\.[0-9]*([eE][0-9]+)?
            StringValue,   //< ("")|('')
            LongString,    //< (""" """)|(''' ''')
            Identifier,    //< [_a-zA-Z][_a-zA-Z0-9]*  <-- Use character categories

            _COUNT,        //< Used for bounds checking - not a token

            // Errors - EOF, Bad Encoding, etc
            Invalid = -1,     //< General error
            EndOfFile = -2,   //< No more characters in the stream
            BadEncoding = -3, //< The utf8::iterate() function failed to decode the stream

            // String Parsing Errors
            UnexpectedStringEOL = -4, //< An EOL was encountered while parsing a string
            UnexpectedStringEOF = -5, //< An EOF was encountered while parsing a string
            BadEscapeHexDigit = -6,   //< Encountered a bad hex digit in a \x, \u, or \U escape
            UnexpectedEscapeEOL = -7, //< An EOL was encountered while parsing a \x, \u, or \U escape
            UnexpectedEscapeEOF = -8  //< An EOF was encountered while parsing a \x, \u, or \U escape
        };

        typedef std::basic_string<char32_t> U32String;

        static const char * Name(Token::Id id);
        static const char32_t * Name32(Token::Id id);

        /**
         * \brief Check if the given Token::Id value holds a string
         * 
         * This is used to check if the value stored in m_Data is a
         * heap-allocated Token::U32String instance.
         * 
         * \param id The Token::Id to check
         * \return If the Token denoted by the Id holds a string value
         */
        static inline constexpr bool IsString(Token::Id id) {
            return (id == Token::Id::String || id == Token::Id::LongString
                || id == Token::Id::Identifier
            );
        }

    public:
        Token();
        Token(Token::Id id, size_t line_no, size_t column);
        Token(const Token & source);
        Token(Token && source);
        ~Token();

        Token & operator=(const Token & source);
        Token & operator=(Token && source);

        Token::U32String * take_string();

        Token::U32String & string();
        const Token::U32String & string() const;
        uint64_t & integer();
        const uint64_t & integer() const;
        double & real();
        const double & real() const;

        Token::Id id() const;
        size_t line() const;
        size_t column() const;
        const char * name() const;
    private:
        size_t m_Line, m_Column;
        Token::Id m_Id;
        union {
            Token::U32String * str_ptr;
            uint64_t integer;
            double real;
        } m_Data;
    };
}

#endif