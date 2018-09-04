
#include "tsbl/utf8.hpp"

#include "utf8proc.h"

using namespace tsbl;

utf8::Category utf8::category(utf8::codepoint_t codepoint) {
	return static_cast<utf8::Category>(utf8proc_category(codepoint));
}

std::pair<int32_t, utf8::codepoint_t> utf8::iterate(const uint8_t *string, int32_t strlen) {
	codepoint_t codepoint = 0;
	auto advance = utf8proc_iterate(string, strlen, &codepoint);
	return std::make_pair(advance, codepoint);
}
