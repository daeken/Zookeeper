#include "NightBeliever.hpp"

static inline uint32_t vmcall(const int call, const void *addr) {
	uint32_t output;
	__asm__ volatile(
		"vmcall\n"
		: "=a"(output) : "a"(call), "d"(addr) : 
	);
	return output;
}

void log(const char *str) {
	vmcall(VMCALL_LOG, str);
}
