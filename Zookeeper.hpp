#pragma once

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <list>
using namespace std;
using boost::format;
using namespace boost::algorithm;

#define bailout(expr) do { if(expr) { cout << "Bailout: " << #expr << " @ " << __FILE__ << " (" << dec << __LINE__ << ")" << endl; exit(1); } } while(0)

#define ONE_KB 1024
#define ONE_MB (1024 * ONE_KB)
#define ONE_GB (1024 * ONE_MB)

#define PAGE_SIZE (4 * ONE_KB)
#define PAGE_MASK (PAGE_SIZE - 1)

#define pagepad(expr) (((expr) & PAGE_MASK) ? ((expr) & ~PAGE_MASK) + PAGE_SIZE : (expr))

#define IN(a, b) (((b).find(a)) != (b).end())

#define RAM_SIZE (128 * ONE_MB)
#define KRAM_SIZE (128 * ONE_MB)

#include "HV.hpp"
#ifdef MAC
#include "Mac/HVMac.hpp"
#endif

#include "Shared.hpp"
#include "Hypercall.hpp"
#include "xbetypes.hpp"
#include "Xbe.hpp"
#include "Cpu.hpp"
#include "Gpu.hpp"
#include "HandleManager.hpp"
#include "PageManager.hpp"
#include "ThreadManager.hpp"
#include "Ioctls.hpp"
#include "IOManager.hpp"
#include "Debugger.hpp"
#include "Box.hpp"
