#pragma once

#include "NightBeliever.hpp"
#include "../vmcalls.hpp"

void log(const char *fmt, ...);
#define printf log

void *map(void *virt_base, uint32_t count);
void unmap(void *virt_base, uint32_t count);
XbeHeader_t *get_xbebase();
