
#include <stdint.h>

#include <iostream>

#include "tsbl/lexer.hpp"
#include "tsbl/utf8.hpp"

using namespace tsbl;

const char * _g_default_string_stream =
    "+ - *\ntrue try throw try_it identifier_1\n";
const char * _g_hex_digits = "0123456789ABCDEF";

void insert_hex_escape(std::ostream & buffer, char value) {
    buffer << "\\x" << _g_hex_digits[value << 4] <<
        _g_hex_digits[value & 0x0F];
}

std::ostream & repr_utf32(std::ostream & buffer, Token::U32String & u32str) {
    char bytes[4];

    for (size_t i = 0; i < u32str.length(); ++i) {
        if (u32str[i] < 128) {
            buffer << ((char)u32str[i]);
        }
        else {
            bytes[0] = u32str[i] & 0xFF;
            bytes[1] = (u32str[i] << 8) & 0xFF;
            bytes[2] = (u32str[i] << 16) & 0xFF;
            bytes[3] = (u32str[i] << 24) & 0xFF;

            if (bytes[0] == 0 && bytes[1] == 0) {
                if (bytes[2] == 0) {
                    insert_hex_escape(buffer, bytes[3]);
                }
                else {
                    insert_hex_escape(buffer, bytes[2]);
                    insert_hex_escape(buffer, bytes[3]);
                }
            }
            else {
                insert_hex_escape(buffer, bytes[0]);
                insert_hex_escape(buffer, bytes[1]);
                insert_hex_escape(buffer, bytes[2]);
                insert_hex_escape(buffer, bytes[3]);
            }
        }
    }
    return buffer;
}

void lex_data(utf8::Reader & reader) {
    tsbl::Lexer lexer;
    lexer.read(reader);
    Token tok;
    std::cout << "Token Stream:" << std::endl;
    while ((tok = lexer.next()).id() >= 0) {
        std::cout << "  " << Token::Name(tok.id());
        if (Token::IsString(tok.id())) {
            std::cout << ": ";
            repr_utf32(std::cout, tok.string());
        }
        else {
            switch (tok.id()) {
            case Token::IntegerValue:
                std::cout << tok.integer();
                break;
            case Token::RealValue:
                std::cout << tok.real();
                break;
            default:
                break;
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Running program with default string buffer" << std::endl;
        utf8::StringReader sr((uint8_t *)_g_default_string_stream);
        lex_data(sr);
    }
    else {
        std::cout << "Running program with file " << argv[1] << std::endl;
        utf8::FileReader fr(argv[1]);
        lex_data(fr);
    }
    
    return 0;
}
