#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();

	Cpu *cpu;
	Gpu *gpu;
	Hypercall *hypercall;
	HandleManager *hm;
	PageManager *pm;
	ThreadManager *tm;
	IOManager *io;
	Debugger *debugger;

	uint32_t xbebase;
};

extern Box *box;
