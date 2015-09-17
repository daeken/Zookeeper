#include "Zookeeper.hpp"

int vmcall_dispatch(Cpu &cpu, uint32_t call, uint32_t addr) {
	switch(call) {
		case VMCALL_LOG: {
			string str = "";
			char c;
			do {
				cpu.read_memory(addr++, 1, &c);
				if(c == 0)
					break;
				str += c;
			} while(true);
			cout << "Guest Log: \"" << str << "\"" << endl;
			break;
		}
		default:
			cout << "Unknown VMCall: 0x" << hex << call << " -- " << hex << addr << endl;
			return 1;
	}
	return 0;
}