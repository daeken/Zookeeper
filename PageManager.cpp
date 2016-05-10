#include "Zookeeper.hpp"

PageManager::PageManager() {
	virtgroup_t group;
	group.start = 0x10000000;
	group.end = 0xc0000000;
	group.count = (group.end - group.start) / 4096;
	virtGroups.push_back(group);
}

void PageManager::add_region(uint32_t base, uint32_t size) {
	if(size & 0xFFF)
		size = (size & ~0xFFF) + 4096;

	for(uint32_t addr = 0; addr < base + size; addr += 4096)
		freePhysPages.push_back(addr);
}

uint32_t PageManager::map(uint32_t base, uint32_t count) {
	if(base == 0)
		base = box->pm->alloc_virt(count);
	
	for(auto i = 0; i < count; ++i) {
		auto virt = base + i * 4096;
		if(!box->cpu->is_mapped(virt))
			box->cpu->map_pages(virt, box->pm->alloc_phys(), 1);
	}

	return base;
}

void PageManager::unmap(uint32_t base, uint32_t count) {
	auto addr = base;
	for(auto i = 0; i < count; ++i) {
		box->pm->free_phys(box->cpu->virt2phys(addr));
		addr += 4096;
	}
	box->pm->free_virt(base, count);
}

uint32_t PageManager::alloc_phys() {
	bailout(freePhysPages.size() == 0);

	auto page = freePhysPages.front();
	freePhysPages.pop_front();
	return page;
}

void PageManager::free_phys(uint32_t page) {
	freePhysPages.push_back(page);
}

uint32_t PageManager::alloc_virt(uint32_t count) {
	for(auto iter = virtGroups.begin(); iter != virtGroups.end(); ++iter) {
		if(iter->count >= count) {
			auto start = iter->start;
			if(iter->count == count)
				virtGroups.erase(iter);
			else {
				iter->count -= count;
				iter->start += count * 4096;
			}
			return start;
		}
	}

	bailout(true); // Couldn't find a group with a large enough count
}

void PageManager::free_virt(uint32_t start, uint32_t count) {
	auto end = start + count * 4096;
	if(virtGroups.size() == 0) {
		virtgroup_t group;
		group.start = start;
		group.end = end;
		group.count = count;
		virtGroups.push_back(group);
	} else{
		for(auto iter = virtGroups.begin(); iter != virtGroups.end(); ++iter) {
			if(iter->start == end) {
				iter->start = start;
				iter->count += count;
				break;
			} else if(iter->start > end) {
				virtgroup_t group;
				group.start = start;
				group.end = end;
				group.count = count;
				virtGroups.insert(iter, group);
				break;
			} else if(iter->end == start) {
				iter->end = end;
				iter->count += count;

				auto group = *iter;
				++iter;
				if(iter->start == end) {
					iter->start = group.start;
					iter->count += group.count;
					virtGroups.erase(--iter);
				}

				break;
			}
		}
	}
}

void PageManager::debug() {
	for(auto group : virtGroups) {
		cout << "Group of " << dec << group.count << " pages @ " << hex << group.start << endl;
	}
	cout << endl;
}
