#pragma once

#include "NightBeliever.hpp"
#include "../vmcalls.hpp"

void log(const char *fmt, ...);
#define printf log
