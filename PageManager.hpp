#pragma once

#include "ZooKeeper.hpp"

typedef struct virtgroup {
	uint32_t start, end, count;
} virtgroup_t;

class PageManager {
public:
	PageManager();
	void add_region(uint32_t base, uint32_t size);

	uint32_t alloc_phys();
	void free_phys(uint32_t page);

	uint32_t alloc_virt(uint32_t count);
	void free_virt(uint32_t start, uint32_t count);

	void debug();

private:
	list<uint32_t> freePhysPages;
	list<virtgroup_t> virtGroups;
};