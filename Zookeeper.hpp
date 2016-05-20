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

#define pagepad(expr) (((expr) & 0xFFF) ? ((expr) & ~0xFFF) + 4096 : (expr))

#define IN(a, b) (((b).find(a)) != (b).end())

#define RAM_SIZE 128*1024*1024
#define KRAM_SIZE 128*1024*1024

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
