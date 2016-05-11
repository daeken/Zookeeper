#pragma once

typedef void (*thread_ep_t)(uint32_t, uint32_t);

static inline uint32_t vmcall(const int call, volatile const uint32_t addr) {
	uint32_t output;
	__asm__ volatile(
		"vmcall\n"
		: "=a"(output) : "a"(call), "d"(addr) : 
	);
	return output;
}
