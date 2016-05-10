#include "Zookeeper.hpp"

int vmcall_dispatch(uint32_t call, uint32_t addr) {
	switch(call) {
		case VMCALL_LOG: {
			string str = "";
			do {
				auto c = box->cpu->read_memory<uint8_t>(addr++);
				if(c == 0)
					break;
				str += c;
			} while(true);
			str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
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
			arg.esp -= 12;
			auto tid = box->tm->create(arg.eip, arg.esp);
			box->cpu->write_memory(arg.esp + 4, tid);
			box->cpu->write_memory(arg.esp + 8, arg.arg);
			return tid;
		}
		case VMCALL_QUERYEEPROM: {
			return query_eeprom(addr);
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
	}
	return 0;
}