#pragma once
#include "Zookeeper.hpp"

class Lpc : public Device {
public:
	Lpc();

	uint32_t readMmio(uint32_t addr);
	void writeMmio(uint32_t addr, uint32_t value);

	void readPci(uint32_t reg, void *buffer, uint32_t length);
	void writePci(uint32_t reg, void *buffer, uint32_t length);

	uint32_t readPort(uint32_t port, uint32_t size);
	void writePort(uint32_t port, uint32_t size, uint32_t value);
};
