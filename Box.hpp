#pragma once

#include "ZooKeeper.hpp"

class Box {
public:
	Box();
	void add_mmio(uint32_t base, uint32_t pages, Device *dev);
	void add_port(uint32_t port, Device *dev);
	void add_pci(uint16_t bus, uint16_t slot, Device *dev);

	unique_ptr<Cpu> cpu;
	unique_ptr<Hypercall> hypercall;
	unique_ptr<HandleManager> hm;
	unique_ptr<PageManager> pm;
	unique_ptr<ThreadManager> tm;
	unique_ptr<IOManager> io;
	unique_ptr<Debugger> debugger;

	unique_ptr<Gpu> gpu;
	unique_ptr<Lpc> lpc;
	unique_ptr<Smbus> smbus;

	map<uint32_t, Device *> mmio;
	map<uint32_t, Device *> ports;
	map<uint32_t, Device *> pci;

	uint32_t xbebase;
	uint32_t kernel_entry;
	bool frame_rendered = false; // Trigger return from cpu
};

extern Box *box;
