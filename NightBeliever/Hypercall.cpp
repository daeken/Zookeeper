#include "NightBeliever.hpp"

static inline uint32_t vmcall(const int call, volatile const void *addr) {
	uint32_t output;
	__asm__ volatile(
		"vmcall\n"
		: "=a"(output) : "a"(call), "d"(addr) : 
	);
	return output;
}

void log(const char *fmt, ...) {
	char output[2048];
	va_list arglist;

	va_start(arglist, fmt);
	vsnprintf(output, 2048, fmt, arglist);
	va_end(arglist);

	vmcall(VMCALL_LOG, output);
}

void *map(void *virt_base, uint32_t count) {
	volatile map_pages_t smap;
	smap.virt_base = (uint32_t) virt_base;
	smap.count = count;

	vmcall(VMCALL_MAP, &smap);

	return (void *) smap.virt_base;
}

void unmap(void *virt_base, uint32_t count) {
	volatile unmap_pages_t smap;
	smap.virt_base = (uint32_t) virt_base;
	smap.count = count;

	vmcall(VMCALL_UNMAP, &smap);
}

XbeHeader_t *get_xbebase() {
	return (XbeHeader_t *) vmcall(VMCALL_XBEBASE, NULL);
}
