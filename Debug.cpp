#include "Zookeeper.hpp"

#define MAX_FRAMES 8
void dumpStack() {
	cout << endl;
	cout << "Stack trace:" << endl;

	auto ebp = (uint32_t) box->cpu->rreg(HV_X86_RBP);
	for(auto i = 0; i < MAX_FRAMES; ++i) {
		if(ebp == 0)
			break;
		auto eip = box->cpu->read_memory<uint32_t>(ebp + 4);
		cout << hex << eip << endl;
		ebp = box->cpu->read_memory<uint32_t>(ebp);
	}

	cout << endl;
}