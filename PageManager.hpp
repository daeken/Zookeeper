#pragma once

#include "ZooKeeper.hpp"

typedef struct virtgroup {
	uint32_t start, end, count;
} virtgroup_t;

class PageManager {
public:
	PageManager();
	void add_region(uint32_t base, uint32_t size);

	uint32_t map(uint32_t base, uint32_t count);
	uint32_t map_contiguous(uint32_t base, uint32_t phys_low, uint32_t phys_high, uint32_t count);
	void unmap(uint32_t base, uint32_t count);

	uint32_t alloc_phys(uint32_t count=1, uint32_t phys_low=0, uint32_t phys_high=0xFFFFFFFF);
	void free_phys(uint32_t page);

	uint32_t alloc_virt(uint32_t count, uint32_t low=0);
	void free_virt(uint32_t start, uint32_t count);

	void debug();

private:
	list<uint32_t> freePhysPages;
	list<virtgroup_t> virtGroups;
};