#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();
	void add_mmio(uint32_t base, uint32_t pages, Device *dev);
	void add_port(uint32_t port, Device *dev);
	void add_pci(uint16_t bus, uint16_t slot, Device *dev);

	Cpu *cpu;
	Gpu *gpu;
	Hypercall *hypercall;
	HandleManager *hm;
	PageManager *pm;
	ThreadManager *tm;
	IOManager *io;
	Debugger *debugger;

	map<uint32_t, Device *> mmio;
	map<uint32_t, Device *> ports;
	map<uint32_t, Device *> pci;

	uint32_t xbebase;
};

extern Box *box;
