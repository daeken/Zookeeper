#include "NightBeliever.hpp"

static inline uint32_t vmcall(const int call, const void *addr) {
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

void *map(void *virt_base, void *phys_base, uint32_t count) {
	map_pages_t smap;
	smap.virt_base = (uint32_t) virt_base;
	smap.phys_base = (uint32_t) phys_base;
	smap.count = count;

	vmcall(VMCALL_MAP, &smap);

	return (void *) smap.virt_base;
}

void unmap(void *virt_base, uint32_t count) {
	unmap_pages_t smap;
	smap.virt_base = (uint32_t) virt_base;
	smap.count = count;

	vmcall(VMCALL_UNMAP, &smap);
}
