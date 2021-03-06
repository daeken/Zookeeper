#pragma once

#include <boost/format.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <list>
#include <map>
using namespace std;
using boost::format;

#define bailout(expr) do { if(expr) { cout << "Bailout: " << #expr << " @ " << __FILE__ << " (" << dec << __LINE__ << ")" << endl; exit(1); } } while(0)

#define IN(a, b) (((b).find(a)) != (b).end())

// Xbox has 64MB RAM, Chihiro has 128 MB RAM
#define RAM_SIZE (128 * ONE_MB)
#define KRAM_SIZE (128 * ONE_MB)

#define KBASE 0xC0000000

#include "libretro.h"

#include "HV.hpp"
#ifdef MAC
#include "Mac/HVMac.hpp"
#endif

#include "Shared.hpp"
#include "Hypercall.hpp"
#include "xbetypes.hpp"
#include "Xbe.hpp"

#include "Device.hpp"
#include "Cpu.hpp"
#include "Gpu.hpp"
#include "Lpc.hpp"
#include "Smbus.hpp"

#include "HandleManager.hpp"
#include "PageManager.hpp"
#include "ThreadManager.hpp"
#include "Ioctls.hpp"
#include "IOManager.hpp"
#include "Debugger.hpp"
#include "Box.hpp"
