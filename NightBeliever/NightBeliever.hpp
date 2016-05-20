#pragma once

#define halt() do { asm("hlt"); } while(0)
#define NTAPI __attribute__((stdcall))

#include <stdint.h>
#include <stdarg.h>

#include "mini-printf.hpp"
#include "Log.hpp"
#include "C.hpp"
#include "../Shared.hpp"
#include "../xbetypes.hpp"
#include "XboxKernel/nttypes.hpp"
#include "TIB.hpp"
#include "Hypercall.hpp"
#include "liballoc.hpp"
#include "XboxKernel/KernelThunk.hpp"
#include "XboxKernel/Kernel.hpp"
