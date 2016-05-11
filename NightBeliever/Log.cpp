#include "NightBeliever.hpp"

void log(const char *fmt, ...) {
	char output[2048];
	va_list arglist;

	va_start(arglist, fmt);
	vsnprintf(output, 2048, fmt, arglist);
	va_end(arglist);

	log_(output);
}
