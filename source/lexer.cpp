
#include "tsbl/lexer.hpp"

using namespace tsbl;

extern const bool _g_CategoryIdentifier[];
extern const bool _g_CategoryIdentifier_Start[];

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
    m_StartLine = m_Line;
    m_StartColumn = m_CharColumn;

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
            return Token(Token::Id::Power, m_Line, m_StartColumn);
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
            return Token(Token::Id::Equals, m_Line, m_StartColumn);
        }
        return Token(Token::Id::Assign, m_Line, m_CharColumn);
    case '!':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::NotEquals, m_Line, m_StartColumn);
        }
        return Token(Token::Id::Not, m_Line, m_CharColumn);
    case '>':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::GreaterEquals, m_Line, m_StartColumn);
        }
        else if (peek_cp() == '>') {
            next_cp();
            return Token(Token::Id::RShift, m_Line, m_StartColumn);
        }
        return Token(Token::Id::Greater, m_Line, m_CharColumn);
    case '<':
        if (peek_cp() == '=') {
            next_cp();
            return Token(Token::Id::LessEquals, m_Line, m_StartColumn);
        }
        else if (peek_cp() == '<') {
            next_cp();
            return Token(Token::Id::LShift, m_Line, m_StartColumn);
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
                return Token(Token::Id::Try, m_Line, m_StartColumn);
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
                        return Token(Token::Id::Throw, m_Line, m_StartColumn);
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

    return Token(Token::Id::Invalid, m_Line, m_CharColumn);
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
    size_t length;
    for (length = 0; start_val[length] != '\0'; ++length);

    Token token(Token::Id::Identifier, line(), column() - (length - 1));
    for (size_t i = 0; i < length; ++i) {
        token.string() += (utf8::codepoint_t)(start_val[i]);
    }
    consume_identifier(token);
    
    return token;
}

Token Lexer::consume_identifier(utf8::codepoint_t pt) {
    Token token(Token::Id::Identifier, line(), column() - 1);
    token.string() += pt;
    consume_identifier(token);
    return token;
}

void Lexer::consume_identifier(Token & token) {
    while (identifier(peek_cp())) {
        token.string() += peek_cp();
        next_cp();
    }
}

Token Lexer::consume_string(bool dbl, bool longstr) {
    size_t endsize = (longstr ? 3 : 1);
    size_t count = 0;
    size_t line_start = line();
    size_t char_start = column() - (count - 1);
    utf8::codepoint_t quote_cp = (utf8::codepoint_t)(dbl ? '"' : '\'');
    utf8::codepoint_t pt;
    Token::U32String data;
    while (count < endsize) {
        if (m_Next < 0) {
            return error(next_cp());
        }

        count = (m_Next == quote_cp ? count + 1 : 0);
        if (m_Next == '\\') {
            next_cp();
            switch (m_Next) {
            case 'a':
                data += (utf8::codepoint_t)'\a';
                break;
            case 'b':
                data += (utf8::codepoint_t)'\b';
                break;
            case 'n':
                data += (utf8::codepoint_t)'\n';
                break;
            case 'r':
                data += (utf8::codepoint_t)'\r';
                break;
            case 't':
                data += (utf8::codepoint_t)'\t';
                break;
            case 'u':
                pt = consume_escape(4);
                if (pt < 0) {
                    return error(pt);
                }
                data += pt;
                break;
            case 'U':
                data += consume_escape(8);
                if (pt < 0) {
                    return error(pt);
                }
                break;
            case 'v':
                data += (utf8::codepoint_t)'\v';
                break;
            case 'x':
                pt = consume_escape(2);
                if (pt < 0) {
                    return error(pt);
                }
                data += consume_escape(2);
                break;
            case '\'':
                data += (utf8::codepoint_t)'\'';
                break;
            case '\"':
                data += (utf8::codepoint_t)'\"';
                break;
            }
        }
        else if (m_Next == '\n' || m_Next == '\r') {
            return error(utf8::Codepoint::UnexpectedStringEOL);
        }
        else {
            data += m_Next;
        }
        next_cp();
    }
    Token::Id id = (longstr ? Token::Id::LongString : Token::Id::String);
    Token tok(id, line_start, char_start);
    tok.string().swap(data); //< Move data into the Token value
    return tok;
}

Token Lexer::consume_numeric() {
    uint64_t whole = 0;
    uint64_t fractional = 0;
    uint64_t exponent = 0;
    Token::Id t_id = Token::Id::Integer;
    while (m_Next >= '0' && m_Next <= '9') {
        whole = whole * 10 + (m_Next - '0');
        next_cp();
    }
    if (m_Next = '.') {
        t_id = Token::Id::Real;
        next_cp();
        while (m_Next >= '0' && m_Next <= '9') {
            fractional = fractional * 10 + (m_Next - '0');
            next_cp();
        }
    }
    if (m_Next == 'e' || m_Next == 'E') {
        t_id = Token::Id::Real;
        next_cp();
        while (m_Next >= '0' && m_Next <= '9') {
            exponent = exponent * 10 + (m_Next - '0');
            next_cp();
        }
    }
    return Token(Token::Id::Invalid, 0, 0);
}

utf8::codepoint_t Lexer::consume_escape(size_t hex_digits) {
    utf8::codepoint_t pt = 0;
    next_cp();
    for (size_t i = 0; i < 4; ++i) {
        if (m_Next >= '0' && m_Next <= '9') {
            pt = pt * 16 + (m_Next - '0');
        }
        else if (m_Next >= 'a' && m_Next <= 'f') {
            pt = pt * 16 + (m_Next - 'a' + 10);
        }
        else if (m_Next >= 'A' && m_Next <= 'F') {
            pt = pt * 16 + (m_Next - 'A' + 10);
        }
        else {
            if (m_Next == '\n' || m_Next == '\r') {
                return utf8::Codepoint::UnexpectedEscapeEOL;
            }
            if (m_Next == utf8::Codepoint::EndOfFile) {
                return utf8::Codepoint::UnexpectedEscapeEOF;
            }
            return utf8::Codepoint::BadEscapeHexDigit;
        }
    }
    return pt;
}

/**
 * \brief Generate the appropriate error Token from the given error codepoint
 * 
 * \param _line The start line number
 * \param _column The start character column
 * \return A Token instance with the correct error Token::Id
 */
Token Lexer::error(utf8::codepoint_t pt) const {
    switch (pt) {
    case utf8::Codepoint::EndOfFile:
        return Token(Token::Id::EndOfFile, m_StartLine, m_StartColumn);
    case utf8::Codepoint::Invalid:
        return Token(Token::Id::BadEncoding, m_Line, m_CharColumn);
    case utf8::Codepoint::UnexpectedStringEOL:
        return Token(Token::Id::UnexpectedEscapeEOL, m_StartLine, m_StartColumn);
    case utf8::Codepoint::UnexpectedStringEOF:
        return Token(Token::Id::UnexpectedStringEOF, m_Line, m_CharColumn);
    }
    // This shouldn't happen
    return Token(Token::Id::Invalid, m_Line, m_CharColumn);
}

//===========================================================================
// Data definitions
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
