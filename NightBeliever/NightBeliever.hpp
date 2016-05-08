#pragma once

#include <stdint.h>
#include "mini-printf.hpp"
#include "Hypercall.hpp"
#include "Interrupts.hpp"
#include "liballoc.hpp"

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
