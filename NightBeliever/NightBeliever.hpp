#pragma once

#define halt() do { asm("hlt"); } while(0)

#include <stdint.h>
#include "mini-printf.hpp"
#include "../xbetypes.hpp"
#include "Hypercall.hpp"
#include "Interrupts.hpp"
#include "liballoc.hpp"
#include "KernelThunk.hpp"

inline void *operator new(uint32_t size) {
	return malloc(size);
}

inline void *operator new[](uint32_t size) {
	return malloc(size);
}

inline void operator delete(void *ptr) {
	free(ptr);
}

inline void operator delete[](void *ptr) {
	free(ptr);
}
