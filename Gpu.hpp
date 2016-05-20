#pragma once
#include "Zookeeper.hpp"

class Gpu : public MMIOReceiver {
public:
	Gpu();
	
	uint32_t read(uint32_t addr);
	void write(uint32_t addr, uint32_t value);
};
