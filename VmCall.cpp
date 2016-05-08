#include "Zookeeper.hpp"

int vmcall_dispatch(uint32_t call, uint32_t addr) {
	switch(call) {
		case VMCALL_LOG: {
			string str = "";
			char c;
			do {
				box->cpu->read_memory(addr++, 1, &c);
				if(c == 0)
					break;
				str += c;
			} while(true);
			cout << "Guest Log: \"" << str << "\"" << endl;
			break;
		}
		case VMCALL_MAP: {
			auto smap = box->cpu->read_memory<map_pages_t>(addr);

			if(smap.virt_base == 0) {
				smap.virt_base = box->pm->alloc_virt(smap.count);
				box->cpu->write_memory(addr, smap);
			}
			
			for(int i = 0; i < smap.count; ++i)
				box->cpu->map_pages(smap.virt_base + i * 4096, box->pm->alloc_phys(), 1);

			break;
		}
		case VMCALL_UNMAP: {
			auto smap = box->cpu->read_memory<unmap_pages_t>(addr);
			auto addr = smap.virt_base;

			for(int i = 0; i < smap.count; ++i) {
				box->pm->free_phys(box->cpu->virt2phys(addr));
				addr += 4096;
			}
			box->pm->free_virt(smap.virt_base, smap.count);

			break;
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
			return 1;
	}
	return 0;
}