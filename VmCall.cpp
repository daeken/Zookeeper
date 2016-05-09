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
			auto arg = box->cpu->read_memory<map_pages_t>(addr);

			auto base = box->pm->map(arg.virt_base, arg.count);
			if(arg.virt_base == 0) {
				arg.virt_base = base;
				box->cpu->write_memory(addr, arg);
			}

			break;
		}
		case VMCALL_UNMAP: {
			auto arg = box->cpu->read_memory<unmap_pages_t>(addr);
			
			box->pm->unmap(arg.virt_base, arg.count);

			break;
		}
		case VMCALL_XBEBASE: {
			return box->xbebase;
		}
		case VMCALL_CREATETHREAD: {
			auto arg = box->cpu->read_memory<create_thread_t>(addr);
			arg.esp -= 8;
			uint32_t ret = 0xFFFFFFFF; // Bad return address
			box->cpu->write_memory(arg.esp, ret);
			box->cpu->write_memory(arg.esp + 4, arg.arg);
			return box->tm->create(arg.eip, arg.esp);
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
	}
	return 0;
}