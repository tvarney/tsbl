
#pragma once
#ifndef TSBL_LEXER_HPP
#define TSBL_LEXER_HPP

#include <stdint.h>
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

			// Errprs - EOF, Bad Encoding
			EndOfFile = -1,
            BadEncoding = -2
		};
	public:
		Token(Token::Id id, size_t line_no, size_t column);
		~Token();

		Token::Id id() const;
		size_t line() const;
		size_t column() const;
	private:
		size_t m_Line, m_Column;
		Token::Id m_Id;
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
	private:
		size_t m_CharColumn, m_Line;
		utf8::codepoint_t m_Current, m_Next;
        utf8::Reader * m_Reader;

        Token::Id next_impl();
	};
}

#endif
