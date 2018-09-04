
#include "tsbl/lexer.hpp"

using namespace tsbl;

extern const bool _g_CategoryIdentifier[];
extern const bool _g_CategoryIdentifier_Start[];
extern const char * _g_TokenName[];

const char * Token::Name(Token::Id id) {
    if (id < 0) {
        if (id == Token::Id::EndOfFile) {
            return "EOF";
        }
        return "BadEncoding";
    }
    if (id >= Token::Id::_COUNT) {
        return "BadTokenId";
    }

    return _g_TokenName[id];
}

Token::Token(Token::Id id, size_t line_no, size_t column) :
    m_Line(line_no), m_Column(column), m_Id(id), m_StringData(nullptr)
{ }

Token::Token(const Token & source) :
    m_Line(source.line()), m_Column(source.column()), m_Id(source.id()),
    m_StringData(nullptr)
{
    if (source.has_value()) {
        make_value(source.value());
    }
}

Token::Token(Token && source) :
    m_Line(source.line()), m_Column(source.column()), m_Id(source.id()),
    m_StringData(source.m_StringData)
{
    source.m_StringData = nullptr;
}

Token::~Token() {
    if (m_StringData != nullptr) {
        delete m_StringData;
    }
}

Token & Token::operator=(const Token & source) {
    m_Line = source.line();
    m_Column = source.column();
    m_Id = source.id();
    if (source.has_value()) {
        make_value(source.value());
    }
    return *this;
}

Token & Token::operator=(Token && source) {
    m_Line = source.line();
    m_Column = source.column();
    m_Id = source.id();
    m_StringData = source.m_StringData;
    source.m_StringData = nullptr;
    return *this;
}

void Token::make_value() {
    if (m_StringData == nullptr) {
        m_StringData = new Token::Utf8String();
    }
}

void Token::make_value(const Token::Utf8String & value) {
    if (m_StringData == nullptr) {
        m_StringData = new Token::Utf8String(value);
    }
    else {
        // If we already have a string, just copy the value
        *m_StringData = value;
    }
}

void Token::make_value(Token::Utf8String && value) {

}

Token::Id Token::id() const {
    return m_Id;
}

size_t Token::line() const {
    return m_Line;
}

size_t Token::column() const {
    return m_Column;
}

const char * Token::name() const {
    return Token::Name(m_Id);
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

bool Lexer::identifier(utf8::codepoint_t pt) const {
    return _g_CategoryIdentifier[utf8::category(pt)];
}

bool Lexer::identifier_start(utf8::codepoint_t pt) const {
    return _g_CategoryIdentifier_Start[utf8::category(pt)];
}

Token Lexer::next() {
    utf8::codepoint_t codepoint = next_cp();

    switch (codepoint) {
    case '\n':
        if (peek_cp() == '\r') {
            next_cp();
        }
        m_CharColumn = 0;
        m_Line += 1;
        return Token(Token::Id::NewLine, m_Line, m_CharColumn);
    case '\r':
        m_CharColumn = 0;
        m_Line += 1;
        return Token(Token::Id::NewLine, m_Line, m_CharColumn);
    case '+':
        return Token(Token::Id::Plus, m_Line, m_CharColumn);
    case '-':
        return Token(Token::Id::Minus, m_Line, m_CharColumn);
    case '*':
        if (peek_cp() == '*') {
            next_cp();
            return Token(Token::Id::Power, m_Line, m_CharColumn - 1);
        }
        return Token(Token::Id::Multiply, m_Line, m_CharColumn);
    case '/':
        return Token(Token::Id::Divide, m_Line, m_CharColumn);
    case '(':
        return Token(Token::Id::OpenParen, m_Line, m_CharColumn);
    case ')':
        return Token(Token::Id::CloseParen, m_Line, m_CharColumn);
    case '[':
        return Token(Token::Id::OpenBracket, m_Line, m_CharColumn);
    case ']':
        return Token(Token::Id::CloseBracket, m_Line, m_CharColumn);
    case '{':
        return Token(Token::Id::OpenBrace, m_Line, m_CharColumn);
    case '}':
        return Token(Token::Id::CloseBrace, m_Line, m_CharColumn);
    case '.':
        return Token(Token::Id::Access, m_Line, m_CharColumn);
    case '=':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::Equals, m_Line, m_CharColumn - 1);
        }
        return Token(Token::Id::Assign, m_Line, m_CharColumn);
    case '!':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::NotEquals, m_Line, m_CharColumn - 1);
        }
        return Token(Token::Id::Not, m_Line, m_CharColumn);
    case '>':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::GreaterEquals, m_Line, m_CharColumn - 1);
        }
        else if (peek_cp() == '>') {
            next_cp();
            return Token(Token::Id::RShift, m_Line, m_CharColumn - 1);
        }
        return Token(Token::Id::Greater, m_Line, m_CharColumn);
    case '<':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::LessEquals, m_Line, m_CharColumn - 1);
        }
        else if (peek_cp() == '<') {
            next_cp();
            return Token(Token::Id::LShift, m_Line, m_CharColumn - 1);
        }
        return Token(Token::Id::Less, m_Line, m_CharColumn);
    case 't':
        // Possible values: try, throw
        if (peek_cp() == 'r') {
            next_cp();
            if (peek_cp() == 'y') {
                next_cp();
                if (identifier(peek_cp())) {
                    return consume_identifier("try");
                }
                return Token(Token::Id::Try, m_Line, m_CharColumn - 2);
            }
            return consume_identifier("tr");
        }
        else if (peek_cp() == 'h') {
            next_cp();
            if (peek_cp() == 'r') {
                next_cp();
                if (peek_cp() == 'o') {
                    next_cp();
                    if (peek_cp() == 'w') {
                        next_cp();
                        if (identifier(peek_cp())) {
                            return consume_identifier("throw");
                        }
                        return Token(Token::Id::Throw, m_Line, m_CharColumn - 4);
                    }
                    return consume_identifier("thro");
                }
                return consume_identifier("thr");
            }
            return consume_identifier("th");
        }
        return consume_identifier("t");
    default:
        if (identifier_start(m_Current)) {
            return consume_identifier(m_Current);
        }
    }

    return Token(Token::Id::UnknownSymbol, m_Line, m_CharColumn);
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

Token Lexer::consume_identifier(const char * start_val) {
    return Token(Token::Id::Identifier, line(), column());
}

//===========================================================================
// Data definitions
static const char * _g_TokenName[] = {
    "\\n",

    "+", "-", "/", "*", "**", "(", ")", "[", "]", "{", "}", ".", "=",
    "!", "==", "!=", ">", ">=", "<", "<=", ">>", "<<",

    "true", "false", "null",

    "struct", "class", "public", "private", "protected", "super", "def",
    "return", "for", "while", "if", "else", "elif", "switch", "break",
    "continue",

    "try", "catch", "throw",

    "int", "float", "string", "docstring", "identifier"
};

static const bool _g_CategoryIdentifier[] = {
    false, //< Not assigned
    true,  //< Letter, Uppercase
    true,  //< Letter, Lowercase
    true,  //< Letter, Titlecase
    true,  //< Letter, Modifier
    true,  //< Letter, Other
    true,  //< Mark, Nonspacing
    true,  //< Mark, Spacing Combining
    true,  //< Mark, Enclosing
    true,  //< Number, Decimal Digit
    true,  //< Number, Letter
    false, //< Number, Other
    true,  //< Punctuation, Connector
    false, //< Punctuation, Dash
    false, //< Punctuation, Open
    false, //< Punctuation, Close
    false, //< Punctuation, Initial Quote
    false, //< Punctuation, Final Quote
    false, //< Punctuation, Other
    false, //< Symbol, Math
    false, //< Symbol, Currency
    false, //< Symbol, Modifier
    false, //< Symbol, Other
    false, //< Seperator, Space
    false, //< Seperator, Line
    false, //< Seperator, Paragraph
    false, //< Other, Control
    false, //< Other, Format
    false, //< Other, Surrogate
    false  //< Other, Private Use
};

static const bool _g_CategoryIdentifier_Start[] = {
    false, //< Not assigned
    true,  //< Letter, Uppercase
    true,  //< Letter, Lowercase
    true,  //< Letter, Titlecase
    true,  //< Letter, Modifier
    true,  //< Letter, Other
    false, //< Mark, Nonspacing
    false, //< Mark, Spacing Combining
    false, //< Mark, Enclosing
    false, //< Number, Decimal Digit
    false, //< Number, Letter
    false, //< Number, Other
    true,  //< Punctuation, Connector
    false, //< Punctuation, Dash
    false, //< Punctuation, Open
    false, //< Punctuation, Close
    false, //< Punctuation, Initial Quote
    false, //< Punctuation, Final Quote
    false, //< Punctuation, Other
    false, //< Symbol, Math
    false, //< Symbol, Currency
    false, //< Symbol, Modifier
    false, //< Symbol, Other
    false, //< Seperator, Space
    false, //< Seperator, Line
    false, //< Seperator, Paragraph
    false, //< Other, Control
    false, //< Other, Format
    false, //< Other, Surrogate
    false  //< Other, Private Use
};
