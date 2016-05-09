#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();

	Cpu *cpu;
	PageManager *pm;
	ThreadManager *tm;

	uint32_t xbebase;
};

extern Box *box;
