
#include <stdint.h>

#include <iostream>

#include "tsbl/lexer.hpp"
#include "tsbl/utf8.hpp"

using namespace tsbl;

const char * _g_default_string_stream = "+ - *\ntrue try try_it\n";

void lex_data(utf8::Reader & reader) {
    tsbl::Lexer lexer;
    lexer.read(reader);
    Token tok;
    std::cout << "Token Stream:" << std::endl;
    while ((tok = lexer.next()).id() >= 0) {
        std::cout << "  " << Token::Name(tok.id()) << std::endl;
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
