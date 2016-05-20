#include "Zookeeper.hpp"

PageManager::PageManager() {
	virtgroup_t group;
	group.start = 0x10000000;
	group.end = KBASE;
	group.count = (group.end - group.start) / PAGE_SIZE;
	virtGroups.push_back(group);
}

void PageManager::add_region(uint32_t base, uint32_t size) {
	if(size & PAGE_MASK)
		size = (size & ~PAGE_MASK) + PAGE_SIZE;

	for(uint32_t addr = 0; addr < base + size; addr += PAGE_SIZE)
		freePhysPages.push_back(addr);
}

uint32_t PageManager::map(uint32_t base, uint32_t count) {
	if(base == 0)
		base = box->pm->alloc_virt(count);
	
	for(auto i = 0; i < count; ++i) {
		auto virt = base + i * PAGE_SIZE;
		if(!box->cpu->is_mapped(virt))
			box->cpu->map_pages(virt, box->pm->alloc_phys(), 1);
	}

	return base;
}

uint32_t PageManager::map_contiguous(uint32_t base, uint32_t phys_low, uint32_t phys_high, uint32_t count) {
	if(base == 0)
		base = box->pm->alloc_virt(count, 0x80000000); // D3D wants a higher half address

	auto phys_base = box->pm->alloc_phys(count, phys_low, phys_high);
	box->cpu->map_pages(base, phys_base, count);
	
	return base;
}

void PageManager::unmap(uint32_t base, uint32_t count) {
	auto addr = base;
	for(auto i = 0; i < count; ++i) {
		box->pm->free_phys(box->cpu->virt2phys(addr));
		addr += PAGE_SIZE;
	}
	box->pm->free_virt(base, count);
}

uint32_t PageManager::alloc_phys(uint32_t count, uint32_t phys_low, uint32_t phys_high) {
	bailout(freePhysPages.size() < count);

	if(count == 1) {
		for(auto iter = freePhysPages.begin(); iter != freePhysPages.end(); ++iter) {
			if(phys_low < *iter && phys_high >= (*iter + PAGE_SIZE)) {
				auto page = *iter;
				freePhysPages.erase(iter);
				return page;
			}
		}
		bailout(true); // Could not find page in range
	} else {
		for(auto base : freePhysPages) {
			if(!(phys_low < base && (base + count * PAGE_SIZE) <= phys_high))
				continue;
			// This is the least efficient search ever.
			auto found = true;
			for(auto i = 1; i < count; ++i) {
				if(find(freePhysPages.begin(), freePhysPages.end(), base + i * PAGE_SIZE) == freePhysPages.end()) {
					found = false;
					break;
				}
			}
			if(found) {
				for(auto i = 0; i < count; ++i) {
					auto iter = find(freePhysPages.begin(), freePhysPages.end(), base + i * PAGE_SIZE);
					freePhysPages.erase(iter);
				}
				return base;
			}
		}
		bailout(true); // Could not find enough contiguous pages in range
	}
}

void PageManager::free_phys(uint32_t page) {
	freePhysPages.push_back(page);
}

uint32_t PageManager::alloc_virt(uint32_t count, uint32_t low) {
	for(auto iter = virtGroups.begin(); iter != virtGroups.end(); ++iter) {
		if(iter->count >= count && iter->end > low + count) {
			if(iter->start >= low) {
				auto start = iter->start;
				if(iter->count == count)
					virtGroups.erase(iter);
				else {
					iter->count -= count;
					iter->start += count * PAGE_SIZE;
				}
				return start;
			} else {
				auto off = low - iter->start;
				auto coff = off / PAGE_SIZE;
				auto start = low;
				if(iter->count == coff + count) {
					iter->count -= count;
					iter->end -= count * PAGE_SIZE;
				} else {
					auto oend = iter->end;
					auto ocount = iter->count;
					iter->count = coff;
					iter->end = low;
					virtgroup_t group;
					group.count = ocount - count - coff;
					group.start = low + count * PAGE_SIZE;
					group.end = oend;
					virtGroups.insert(++iter, group);
				}
				return start;
			}
		}
	}

	bailout(true); // Couldn't find a group with a large enough count
}

void PageManager::free_virt(uint32_t start, uint32_t count) {
	auto end = start + count * PAGE_SIZE;
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
