
#pragma once
#ifndef TSBL_LEXER_HPP
#define TSBL_LEXER_HPP

#include <stdint.h>
#include "tsbl/utf8.hpp"

namespace tsbl {
	class Token {
	public:
		enum Id : int32_t {
			// Constant Size Tokens
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
			Equals,        //< ==
			NotEquals,     //< !=
			Greater,       //< >
			GreaterEquals, //< >=
			Less,          //< <
			LessEquals,    //< <=

			// Keywords
			Class,         //< class
			Public,        //< public
			Private,       //< private
			Protected,     //< protected
			Super,         //< super
			Struct,        //< struct

			Integer,       //< [0-9]+
			Float,         //< [0-9]+\.[0-9]*([eE][0-9]+)?
			String,        //< ("")|('')
			LongString,    //< (""" """)|(''' ''')
			Identifier,    //< [_a-zA-Z][_a-zA-Z0-9]*  <-- Use character categories

			// Special - EOF
			EndOfFile = -1
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

		size_t column() const;
		size_t line() const;

		Token next();

		utf8::codepoint_t next_cp();
		utf8::codepoint_t current_cp() const;
		utf8::codepoint_t peek_cp() const;
	private:
		size_t m_CharColumn, m_Line;
		utf8::codepoint_t m_Current, m_Next;
	};
}

#endif
