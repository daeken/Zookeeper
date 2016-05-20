#pragma once

#include "Zookeeper.hpp"

class MMIOReceiver {
public:
	virtual uint32_t read(uint32_t addr) = 0;
	virtual void write(uint32_t addr, uint32_t value) = 0;

	map<uint32_t, void *> buffers;
};

class Cpu {
public:
	Cpu(uint8_t *ram, uint8_t *kram);
	~Cpu();
	void run(uint32_t eip);
	void map_pages(uint32_t virt, uint32_t phys, uint32_t count, bool present=true);
	void map_io(uint32_t base, uint32_t pages, MMIOReceiver *recv);
	void flip_page(uint32_t base, bool val);

	uint32_t virt2phys(uint32_t addr);
	bool is_mapped(uint32_t addr);
	void read_memory(uint32_t addr, uint32_t size, void *buffer);
	template<typename T> T read_memory(uint32_t addr) {
		T value;
		read_memory(addr, sizeof(T), &value);
		return value;
	}

	void write_memory(uint32_t addr, uint32_t size, void *buffer);
	template<typename T> void write_memory(uint32_t addr, T value) {
		write_memory(addr, sizeof(T), &value);
	}

	void invalidate_tlb() {
		hv->invalidate_tlb();
	}

	HV *hv;
	uint8_t *mem, *kmem;
	int single_step = 0;
	bool stop = false;
	bool break_in = false;

	map<uint32_t, MMIOReceiver *> mmio;
};
