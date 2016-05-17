#pragma once

#include "Zookeeper.hpp"

class Cpu {
public:
	Cpu(uint8_t *ram, uint8_t *kram);
	~Cpu();
	void run(uint32_t eip);
	void map_pages(uint32_t virt, uint32_t phys, uint32_t count);

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
};
