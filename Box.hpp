#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();

	Cpu *cpu;
	PageManager *pm;

	uint32_t xbebase;
};

extern Box *box;
