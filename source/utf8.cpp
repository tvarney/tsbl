
#include "tsbl/utf8.hpp"

#include "utf8proc.h"

using namespace tsbl;

utf8::Category utf8::category(utf8::codepoint_t codepoint) {
	return static_cast<utf8::Category>(utf8proc_category(codepoint));
}
