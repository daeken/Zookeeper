#pragma once

#define halt() do { asm("hlt"); } while(0)
#define NTAPI __attribute__((stdcall))

#include <stdint.h>
#include "mini-printf.hpp"
#include "../xbetypes.hpp"
#include "TIB.hpp"
#include "Hypercall.hpp"
#include "Interrupts.hpp"
#include "liballoc.hpp"
#include "nttypes.hpp"
#include "KernelThunk.hpp"
#include "Kernel.hpp"

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
