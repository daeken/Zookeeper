#include "Zookeeper.hpp"

// Comment to enable break_in()s for this file
#define break_in() do { } while(0)

Lpc::Lpc() {
	box->add_pci(1, 0, this);
}

uint32_t Lpc::readMmio(uint32_t addr) {
	switch(addr) {
		default:
			cout << format("Lpc::readMmio(0x%08x)") % addr << endl;
			break_in();
			return 0;
	}
}

void Lpc::writeMmio(uint32_t addr, uint32_t value) {
	cout << format("Lpc::writeMmio(0x%08x, 0x%08x)") % addr % value << endl;
	break_in();
}

void Lpc::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Lpc::readPci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

void Lpc::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Lpc::writePci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

uint32_t Lpc::readPort(uint32_t port, uint32_t size) {
	cout << format("Lpc::readPort(0x%04x, %i)") % port % size << endl;
	break_in();
	return 0;
}

void Lpc::writePort(uint32_t port, uint32_t size, uint32_t value) {
	cout << format("Lpc::writePort(0x%04x, %i, 0x%x)") % port % size % value << endl;
	break_in();
}
