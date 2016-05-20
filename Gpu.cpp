#include "Zookeeper.hpp"

Gpu::Gpu() {
	box->cpu->map_io(0xFD000000, 16, (MMIOReceiver *) this);
	box->cpu->map_io(0xFD100000, 1, (MMIOReceiver *) this);
	box->cpu->map_io(0xFD600000, 1, (MMIOReceiver *) this);
}

uint32_t Gpu::read(uint32_t addr) {
	cout << format("Gpu::read(0x%08x)") % addr << endl;
	return 0;
}

void Gpu::write(uint32_t addr, uint32_t value) {
	cout << format("Gpu::write(0x%08x, 0x%08x)") % addr % value << endl;
}
