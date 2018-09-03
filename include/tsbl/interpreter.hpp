
#pragma once
#ifndef TSBL_INTERPRETER_HPP
#define TSBL_INTERPRETER_HPP

#include <stdint.h>

namespace tsbl {
    class Interpreter {
    public:
        Interpreter();
        ~Interpreter();

		int32_t category(int32_t codepoint) const;
    };
}

#endif
