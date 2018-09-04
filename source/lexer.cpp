
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

void Lexer::read(utf8::Reader & reader) {
    m_Reader = &reader;
}

size_t Lexer::column() const {
    return m_CharColumn;
}

size_t Lexer::line() const {
    return m_Line;
}

Token Lexer::next() {
    return Token(next_impl(), m_Line, m_CharColumn);
}

Token::Id Lexer::next_impl() {
    utf8::codepoint_t codepoint = next_cp();
    

    switch (codepoint) {
    case '\n':
        if (peek_cp() == '\r') {
            next_cp();
        }
        m_CharColumn = 0;
        m_Line += 1;
        return Token::Id::NewLine;
    case '\r':
        m_CharColumn = 0;
        m_Line += 1;
        return Token::Id::NewLine;
    case '+':
        return Token::Id::Plus;
    case '-':
        return Token::Id::Minus;
    case '*':
        if (peek_cp() == '*') {
            next_cp();
            return Token::Id::Power;
        }
        return Token::Id::Multiply;
    case '/':
        return Token::Id::Divide;
    case '(':
        return Token::Id::OpenParen;
    case ')':
        return Token::Id::CloseParen;
    case '[':
        return Token::Id::OpenBracket;
    case ']':
        return Token::Id::CloseBracket;
    case '{':
        return Token::Id::OpenBrace;
    case '}':
        return Token::Id::CloseBrace;
    case '.':
        return Token::Id::Access;
    case '=':
        if (peek_cp() == '=') {
            next_cp();
            return Token::Id::Equals;
        }
        return Token::Id::Assign;
    case '!':
        if (peek_cp() == '=') {
            next_cp();
            return Token::Id::NotEquals;
        }
        return Token::Id::Not;
    case '>':
        if (peek_cp() == '=') {
            next_cp();
            return Token::Id::GreaterEquals;
        }
        else if (peek_cp() == '>') {
            next_cp();
            return Token::Id::RShift;
        }
        return Token::Id::Greater;
    case '<':
        if (peek_cp() == '=') {
            next_cp();
            return Token::Id::LessEquals;
        }
        else if (peek_cp() == '<') {
            next_cp();
            return Token::Id::LShift;
        }
        return Token::Id::Less;
    }

    return Token::Id::EndOfFile;
}

utf8::codepoint_t Lexer::next_cp() {
    m_Current = m_Next;
    m_Next = m_Reader->next();
    // If our current character isn't negative (an error code), increment the
    // column count.
    if (m_Current >= 0) {
        m_CharColumn += 1;
    }
    return m_Current;
}

utf8::codepoint_t Lexer::current_cp() const {
    return m_Current;
}

utf8::codepoint_t Lexer::peek_cp() const {
    return m_Next;
}
