
#include "tsbl/lexer.hpp"

using namespace tsbl;

Token::Token(Token::Id id, size_t line_no, size_t column) :
	m_Line(line_no), m_Column(column), m_Id(id)
{ }
Token::~Token() { }

Token::Id Token::id() const {
	return m_Id;
}

size_t Token::line() const {
	return m_Line;
}

size_t Token::column() const {
	return m_Column;
}

Lexer::Lexer() :
	m_CharColumn(0), m_Line(0),
	m_Current(utf8::Codepoint::Invalid), m_Next(utf8::Codepoint::Invalid)
{ }

Lexer::~Lexer() { }

size_t Lexer::column() const {
	return m_CharColumn;
}

size_t Lexer::line() const {
	return m_Line;
}

Token Lexer::next() {
	return Token(Token::EndOfFile, m_Line, m_CharColumn);
}

utf8::codepoint_t Lexer::next_cp() {
	return -1;
}

utf8::codepoint_t Lexer::current_cp() const {
	return m_Current;
}

utf8::codepoint_t Lexer::peek_cp() const {
	return m_Next;
}
