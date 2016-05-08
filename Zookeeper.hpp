#pragma once

#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <sys/time.h>
#include <list>
using namespace std;

#define bailout(expr) do { if(expr) { cout << "Bailout: " << #expr << " @ " << __FILE__ << " (" << dec << __LINE__ << ")" << endl; exit(1); } } while(0)

#define pagepad(expr) (((expr) & 0xFFF) ? ((expr) & ~0xFFF) + 4096 : (expr))

#define RAM_SIZE 128*1024*1024
#define KRAM_SIZE 128*1024*1024

#include "xbetypes.hpp"
#include "Xbe.hpp"
#include "Cpu.hpp"
#include "PageManager.hpp"
#include "VmCall.hpp"
#include "vmcalls.hpp"
#include "Box.hpp"
