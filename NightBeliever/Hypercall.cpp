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

void log(const char *fmt, va_list arglist) {
	char output[2048];

	vsnprintf(output, 2048, fmt, arglist);

	vmcall(VMCALL_LOG, output);
}

void *map(void *virt_base, uint32_t count) {
	volatile map_pages_t arg;
	arg.virt_base = (uint32_t) virt_base;
	arg.count = count;

	vmcall(VMCALL_MAP, &arg);

	return (void *) arg.virt_base;
}

void unmap(void *virt_base, uint32_t count) {
	volatile unmap_pages_t arg;
	arg.virt_base = (uint32_t) virt_base;
	arg.count = count;

	vmcall(VMCALL_UNMAP, &arg);
}

XbeHeader_t *get_xbebase() {
	return (XbeHeader_t *) vmcall(VMCALL_XBEBASE, NULL);
}

uint32_t create_thread(thread_ep_t eip, void *esp, uint32_t targ) {
	volatile create_thread_t arg;
	arg.eip = (uint32_t) eip;
	arg.esp = (uint32_t) esp;
	arg.arg = targ;

	return vmcall(VMCALL_CREATETHREAD, &arg);
}
