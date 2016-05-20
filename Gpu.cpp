#include "Zookeeper.hpp"

Gpu::Gpu() {
	box->cpu->map_io(0xFD000000, PAGE_SIZE, (MMIOReceiver *) this); // 16MB address space
}

uint32_t Gpu::read(uint32_t addr) {
	cout << format("Gpu::read(0x%08x)") % addr << endl;

	switch(addr) {
		case 0xfd100200:
			return 3;
		default:
			return addr;
	}
}

void Gpu::write(uint32_t addr, uint32_t value) {
	if(addr > 0xfd700000 && addr <= 0xfd705000)
		return;
	cout << format("Gpu::write(0x%08x, 0x%08x)") % addr % value << endl;
}
