#include "NightBeliever.hpp"

void *memcpy(void *dest, const void *src, size_t n) {
	auto a = (uint8_t *) dest, b = (uint8_t *) src;
	while(n--)
		*(a++) = *(b++);
	return dest;
}

void *memset(void *ptr, int value, size_t num) {
	auto chrs = (uint8_t *) ptr;
	while(num--)
		chrs[num] = (uint8_t) value;
	return ptr;
}
