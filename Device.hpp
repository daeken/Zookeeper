#pragma once

#include "Zookeeper.hpp"

class Device {
public:
	virtual uint32_t readMmio(uint32_t addr) = 0;
	virtual void writeMmio(uint32_t addr, uint32_t value) = 0;

	virtual void readPci(uint32_t reg, void *buffer, uint32_t length) = 0;
	virtual void writePci(uint32_t reg, void *buffer, uint32_t length) = 0;

	virtual uint32_t readPort(uint32_t port, uint32_t size) = 0;
	virtual void writePort(uint32_t port, uint32_t size, uint32_t value) = 0;

	map<uint32_t, void *> mmioBuffers;
};
