
#include "tsbl/token.hpp"

using namespace tsbl;

extern const char * _g_TokenName[];

/**
 * \brief Get the string value of the Token::Id
 * 
 * \param id The Token::Id value to get the name of
 * \return A constant string with the name of the Token
 */
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

/**
 * \brief Create a new Token
 * 
 * \param id The Token::Id of the new Token
 * \param line_no The line number of the start of the token
 * \param column The character column of the start of the token
 */
Token::Token(Token::Id id, size_t line_no, size_t column) :
    m_Line(line_no), m_Column(column), m_Id(id)
{
    switch (id) {
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = new Token::U32String();
        break;
    case Token::Id::Real:
        m_Data.real = 0.0;
        break;
    case Token::Id::Integer:
    default:
        m_Data.integer = 0;
    }
}

/**
 * \brief Create a copy of the given Token
 * 
 * If the source Token instance has a value, the newly created Token instance
 * will have a copy of that value.
 * 
 * \param source The Token instance to copy
 */
Token::Token(const Token & source) :
    m_Line(source.line()), m_Column(source.column()), m_Id(source.id())
{
    switch (m_Id) {
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = new Token::U32String(*source.m_Data.str_ptr);
        break;
    case Token::Id::Real:
        m_Data.real = source.m_Data.real;
        break;
    case Token::Id::Integer:
        m_Data.integer = source.m_Data.integer;
        break;
    default:
        m_Data.integer = 0;
    }
}

/**
 * \brief Move construct a Token instance
 * 
 * This constructor will appropriate any value stored in the rval reference
 * passed to the constructor.
 * 
 * \param source The source Token instance to move data from
 */
Token::Token(Token && source) :
    m_Line(source.line()), m_Column(source.column()), m_Id(source.id())
{
    switch (m_Id) {
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = source.m_Data.str_ptr;
        source.m_Data.str_ptr = nullptr;
        break;
    case Token::Id::Integer:
        m_Data.integer = source.m_Data.integer;
        break;
    case Token::Id::Real:
        m_Data.real = source.m_Data.real;
        break;
    default:
        m_Data.integer = 0;
    }
}

/**
 * \brief Destroy the Token
 * 
 * If the Token still has a value(), the value is destroyed. Use take_value()
 * to move a value out of a token before it is deconstructed.
 */
Token::~Token() {
    if(Token::IsString(m_Id) && m_Data.str_ptr != nullptr) {
        delete m_Data.str_ptr;
        m_Data.str_ptr = nullptr;
    }
}

/**
 * \brief Copy the given Token instance
 * 
 * If the source Token instance has a value and this one does not, this
 * Token instance will create a new value string and copy the source's value.
 * If the source Token instance has a value and this one does as well, this
 * Token instance will replace the value stored in it's value string with a
 * copy of the source's value.
 * 
 * \param source The source Token instance to copy
 * \return A reference to this token
 */
Token & Token::operator=(const Token & source) {
    auto old_id = m_Id;

    m_Line = source.line();
    m_Column = source.column();
    m_Id = source.id();
    switch (m_Id) {
    case Token::Id::String:
    case Token::Id::LongString:
        if(Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            *m_Data.str_ptr = *source.m_Data.str_ptr;
        }
        else {
            m_Data.str_ptr = new Token::U32String(*source.m_Data.str_ptr);
        }
        break;
    case Token::Id::Integer:
        if(Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.integer = source.m_Data.integer;
        break;
    case Token::Id::Real:
        if(Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.real = source.m_Data.real;
        break;
    default:
        if(Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.integer = 0;
    }
    return *this;
}

/**
 * \brief Move the given Token value into this Token
 * 
 * \param source The Token instance to move
 */
Token & Token::operator=(Token && source) {
    auto old_id = m_Id;
    Token::U32String * source_string;
    m_Line = source.line();
    m_Column = source.column();
    m_Id = source.id();
    switch (m_Id) {
    case Token::Id::String:
    case Token::Id::LongString:
        if(Token::IsString(old_id)) {
            source_string = source.m_Data.str_ptr;
            source.m_Data.str_ptr = m_Data.str_ptr;
            m_Data.str_ptr = source_string;
        }
        else {
            m_Data.str_ptr = source.m_Data.str_ptr;
            source.m_Data.str_ptr = nullptr;
        }
        break;
    case Token::Id::Real:
        if (Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.real = source.m_Data.real;
        break;
    case Token::Id::Integer:
        if (Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.integer = source.m_Data.integer;
        break;
    default:
        if (Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.integer = 0;
    }
    return *this;
}

Token::U32String * Token::take_string() {
    Token::U32String * strptr = m_Data.str_ptr;
    m_Data.str_ptr = nullptr;
    return strptr;
}

Token::U32String & Token::string() {
    return *m_Data.str_ptr;
}

const Token::U32String & Token::string() const {
    return *m_Data.str_ptr;
}
uint64_t Token::integer() const {
    return m_Data.integer;
}
double Token::real() const {
    return m_Data.real;
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
