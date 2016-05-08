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

			auto base = box->pm->map(smap.virt_base, smap.count);
			if(smap.virt_base == 0) {
				smap.virt_base = base;
				box->cpu->write_memory(addr, smap);
			}

			break;
		}
		case VMCALL_UNMAP: {
			auto smap = box->cpu->read_memory<unmap_pages_t>(addr);
			
			box->pm->unmap(smap.virt_base, smap.count);

			break;
		}
		case VMCALL_ENTRYPOINT: {
			return box->entrypoint;
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
	}
	return 0;
}