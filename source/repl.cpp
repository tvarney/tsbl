
#include <stdint.h>

#include <iostream>

#include "tsbl/interpreter.hpp"

int main(int argc, char **argv) {
	tsbl::Interpreter interp;
	int32_t codepoint = 50;
	std::cout << "Codepoint " << codepoint << "'s category is " << interp.category(codepoint) << std::endl;
	return -1;
}
