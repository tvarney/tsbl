
#include "tsbl/token.hpp"

using namespace tsbl;

extern const char * _g_TokenName[];
extern const char32_t * _g_TokenName32[];

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
 * \brief Get the char32_t * name of the Token::Id
 * 
 * \param id The Token::Id value to get the name of
 * \return A constant string with the name of the Token
 */
const char32_t * Token::Name32(Token::Id id) {
    if (id < 0) {
        if (id == Token::Id::EndOfFile) {
            return U"EOF";
        }
        return U"BadEncoding";
    }
    if (id >= Token::Id::_COUNT) {
        return U"BadTokenId";
    }
    return _g_TokenName32[id];
}

/**
 * \breif Create a new default token
 */
Token::Token() :
    m_Line(0), m_Column(0), m_Id(Token::Id::Invalid)
{
    m_Data.integer = 0;
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
    case Token::Id::Identifier:
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = new Token::U32String();
        break;
    case Token::Id::RealValue:
        m_Data.real = 0.0;
        break;
    case Token::Id::IntegerValue:
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
    case Token::Id::Identifier:
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = new Token::U32String(*source.m_Data.str_ptr);
        break;
    case Token::Id::RealValue:
        m_Data.real = source.m_Data.real;
        break;
    case Token::Id::IntegerValue:
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
    case Token::Id::Identifier:
    case Token::Id::String:
    case Token::Id::LongString:
        m_Data.str_ptr = source.m_Data.str_ptr;
        source.m_Data.str_ptr = nullptr;
        break;
    case Token::Id::IntegerValue:
        m_Data.integer = source.m_Data.integer;
        break;
    case Token::Id::RealValue:
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
    case Token::Id::IntegerValue:
        if(Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.integer = source.m_Data.integer;
        break;
    case Token::Id::RealValue:
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
    case Token::Id::RealValue:
        if (Token::IsString(old_id) && m_Data.str_ptr != nullptr) {
            delete m_Data.str_ptr;
        }
        m_Data.real = source.m_Data.real;
        break;
    case Token::Id::IntegerValue:
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

    "+", "++", "-", "--", "/", "*", "**", "(", ")", "[", "]", "{", "}", ".",
    "=", "!", "==", "!=", ">", ">=", "<", "<=", ">>", "<<",

    "true", "false", "null",

    "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64",
    "float", "double", "char", "string",

    "struct", "class", "public", "private", "protected", "super", "def",
    "return", "pure", "for", "while", "if", "else", "elif", "switch", "break",
    "continue",

    "try", "catch", "throw",

    "int", "float", "string", "docstring", "identifier"
};

static const char32_t * _g_TokenName32[] = {
    U"\\n",

    U"+", U"++", U"-", U"--", U"/", U"*", U"**", U"(", U")", U"[", U"]", U"{",
    U"}", U".", U"=", U"!", U"==", U"!=", U">", U">=", U"<", U"<=", U">>",
    U"<<",

    U"true", U"false", U"null",

    U"int8", U"int16", U"int32", U"int64", U"uint8", U"uint16", U"uint32",
    U"uint64",
    U"float", U"double", U"char", U"string",

    U"struct", U"class", U"public", U"private", U"protected", U"super", U"def",
    U"return", U"pure", U"for", U"while", U"if", U"else", U"elif", U"switch", U"break",
    U"continue",

    U"try", U"catch", U"throw",

    U"int", U"float", U"string", U"docstring", U"identifier"
};
