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
			map_pages_t smap = box->cpu->read_memory<map_pages_t>(addr);

			cout << "Mapping some pages and shit." << endl;
			cout << hex << smap.virt_base << endl;
			cout << hex << smap.phys_base << endl;
			cout << dec << smap.count << endl;

			smap.virt_base = 0xcafebabe;

			box->cpu->write_memory(addr, smap);
			break;
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
			return 1;
	}
	return 0;
}