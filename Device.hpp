#pragma once

#include "Zookeeper.hpp"

class Device {
public:
	Device(string name);
	virtual uint32_t readMmio(uint32_t addr);
	virtual void writeMmio(uint32_t addr, uint32_t value);

	virtual void readPci(uint32_t reg, void *buffer, uint32_t length);
	virtual void writePci(uint32_t reg, void *buffer, uint32_t length);

	virtual uint32_t readPort(uint32_t port, uint32_t size);
	virtual void writePort(uint32_t port, uint32_t size, uint32_t value);

	string name;
	map<uint32_t, void *> mmioBuffers;
};
