#include "Zookeeper.hpp"

Gpu::Gpu() {
	box->add_pci(1, 0, this);
	box->add_mmio(0xFD000000, 4096, this); // 4096 pages * 4KB == 16MB address space
	box->add_port(0x80c0, this);
}

uint32_t Gpu::readMmio(uint32_t addr) {
	cout << format("Gpu::readMmio(0x%08x)") % addr << endl;

	switch(addr) {
		case 0xfd100200:
			return 3;
		default:
			break_in(true);
			return 0;
	}
}

void Gpu::writeMmio(uint32_t addr, uint32_t value) {
	if(addr > 0xfd700000 && addr <= 0xfd705000)
		return;
	cout << format("Gpu::writeMmio(0x%08x, 0x%08x)") % addr % value << endl;
	break_in(true);
}

void Gpu::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Gpu::readPci(0x%08x, %i)") % reg % length << endl;
	break_in(true);
}

void Gpu::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Gpu::writePci(0x%08x, %i)") % reg % length << endl;
	break_in(true);
}

uint32_t Gpu::readPort(uint32_t port, uint32_t size) {
	cout << format("Gpu::readPort(0x%04x, %i)") % port % size << endl;
	break_in(true);
	return 0;
}

void Gpu::writePort(uint32_t port, uint32_t size, uint32_t value) {
	cout << format("Gpu::writePort(0x%04x, %i, 0x%x)") % port % size % value << endl;
	break_in(true);
}
