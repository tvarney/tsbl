
#include "tsbl/interpreter.hpp"

#define UTF8PROC_STATIC
#include "utf8proc.h"

using namespace tsbl;

Interpreter::Interpreter() { }
Interpreter::~Interpreter() { }

int32_t Interpreter::category(int32_t codepoint) const {
	return utf8proc_category(codepoint);
}
