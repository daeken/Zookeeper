#pragma once
#include "Zookeeper.hpp"

class Smbus : public Device {
public:
	Smbus();

	void readPci(uint32_t reg, void *buffer, uint32_t length);
	void writePci(uint32_t reg, void *buffer, uint32_t length);
};
