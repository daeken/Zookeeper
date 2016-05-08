#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();

	Cpu *cpu;
	PageManager *pm;

	uint32_t entrypoint;
};

extern Box *box;
