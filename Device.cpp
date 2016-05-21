#include "Zookeeper.hpp"

Device::Device(string name) : name(name) {
}

uint32_t Device::readMmio(uint32_t addr) {
	switch(addr) {
		default:
			cout << format("%s::readMmio(0x%08x)") % name % addr << endl;
			break_in();
			return 0;
	}
}

void Device::writeMmio(uint32_t addr, uint32_t value) {
	cout << format("%s::writeMmio(0x%08x, 0x%08x)") % name % addr % value << endl;
	break_in();
}

void Device::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("%s::readPci(0x%08x, %i)") % name % reg % length << endl;
	break_in();
}

void Device::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("%s::writePci(0x%08x, %i)") % name % reg % length << endl;
	break_in();
}

uint32_t Device::readPort(uint32_t port, uint32_t size) {
	cout << format("%s::readPort(0x%04x, %i)") % name % port % size << endl;
	break_in();
	return 0;
}

void Device::writePort(uint32_t port, uint32_t size, uint32_t value) {
	cout << format("%s::writePort(0x%04x, %i, 0x%x)") % name % port % size % value << endl;
	break_in();
}
