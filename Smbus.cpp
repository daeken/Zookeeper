#include "Zookeeper.hpp"

// Comment to enable break_in()s for this file
#define break_in() do { } while(0)

Smbus::Smbus() {
	box->add_pci(1, 1, this);
}

uint32_t Smbus::readMmio(uint32_t addr) {
	switch(addr) {
		default:
			cout << format("Smbus::readMmio(0x%08x)") % addr << endl;
			break_in();
			return 0;
	}
}

void Smbus::writeMmio(uint32_t addr, uint32_t value) {
	cout << format("Smbus::writeMmio(0x%08x, 0x%08x)") % addr % value << endl;
	break_in();
}

void Smbus::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Smbus::readPci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

void Smbus::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Smbus::writePci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

uint32_t Smbus::readPort(uint32_t port, uint32_t size) {
	cout << format("Smbus::readPort(0x%04x, %i)") % port % size << endl;
	break_in();
	return 0;
}

void Smbus::writePort(uint32_t port, uint32_t size, uint32_t value) {
	cout << format("Smbus::writePort(0x%04x, %i, 0x%x)") % port % size % value << endl;
	break_in();
}
