#pragma once

#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <sys/time.h>
using namespace std;

#define bailout(expr) do { if(expr) { cout << "Bailout: " << #expr << endl; exit(1); } } while(0)

#define RAM_SIZE 64*1024*1024
#define KRAM_SIZE 128*1024*1024

#include "Xbe.hpp"
#include "Cpu.hpp"
#include "Kernel.hpp"
#include "VmCall.hpp"
#include "vmcalls.hpp"
