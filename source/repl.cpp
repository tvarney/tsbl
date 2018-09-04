
#include <stdint.h>

#include <iostream>

#include "tsbl/utf8.hpp"

using namespace tsbl::utf8;

void output(codepoint_t pt) {
    Category cat = category(pt);
    std::cout << "Codepoint " << pt << ":" << std::endl
        << "  Category: " << category_name(cat) << " [" << category_id(cat) << "|" << cat << "]" << std::endl;
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        int codepoint = atoi(argv[i]);
        if (codepoint > 0) {
            output(codepoint);
        }
        else {
            char * arg = argv[i];
            while (*arg != '\0') {
                if (*arg == '\\') {
                    arg += 1;
                    switch (*arg) {
                    case 'a':
                        codepoint = '\a';
                        break;
                    case 'b':
                        codepoint = '\b';
                        break;
                    case 'f':
                        codepoint = '\f';
                        break;
                    case 'n':
                        codepoint = '\n';
                        break;
                    case 'r':
                        codepoint = '\r';
                        break;
                    case 't':
                        codepoint = '\t';
                        break;
                    case 'v':
                        codepoint = '\v';
                        break;
                    }
                }
                else {
                    codepoint = *arg;
                }
                arg += 1;
                output(codepoint);
            }
        }
    }
    return 0;
}
