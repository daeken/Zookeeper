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

size_t strlen(const char *str) {
	auto i = 0;
	while(str[i++] != 0);
	return i;
}

int tolower(int c) {
	if(c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}

int toupper(int c) {
	if(c >= 'a' && c <= 'z')
		return c - 'a' + 'A';
	return c;
}

int strcmp(const char *str1, const char *str2) {
	auto a = strlen(str1), b = strlen(str2);
	if(a < b) return -1;
	else if(a > b) return 1;

	for(auto i = 0; i < a; ++i) {
		if(str1[i] < str2[i]) return -1;
		else if(str1[i] > str2[i]) return 1;
	}
	return 0;
}

int stricmp(const char *str1, const char *str2) {
	auto a = strlen(str1), b = strlen(str2);
	if(a < b) return -1;
	else if(a > b) return 1;

	for(auto i = 0; i < a; ++i) {
		auto c1 = tolower(str1[i]), c2 = tolower(str2[i]);
		if(c1 < c2) return -1;
		else if(c1 > c2) return 1;
	}
	return 0;
}

void *operator new(size_t size) {
	return malloc(size);
}

void *operator new[](size_t size) {
	return malloc(size);
}

void operator delete(void *ptr) {
	free(ptr);
}

void operator delete[](void *ptr) {
	free(ptr);
}
