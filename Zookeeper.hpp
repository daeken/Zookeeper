#pragma once

#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <iostream>
#include <iomanip>
using namespace std;

#define bailout(expr) do { if(expr) { cout << "Bailout: " << #expr << endl; exit(1); } } while(0)

#define RAM_SIZE 64*1024*1024

#include "Xbe.hpp"
#include "Kernel.hpp"
#include "Cpu.hpp"
