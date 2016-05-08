#pragma once

#include <stdint.h>
#include "mini-printf.hpp"
#include "Hypercall.hpp"
#include "Interrupts.hpp"
#include "liballoc.hpp"

inline void *operator new[](unsigned long size) {
	return malloc(size);
}
