#include "Zookeeper.hpp"

// Comment to enable break_in()s for this file
#define break_in() do { } while(0)

Smbus::Smbus() : Device("Smbus") {
	box->add_pci(1, 1, this);
}

void Smbus::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Smbus::readPci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

void Smbus::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Smbus::writePci(0x%08x, %i)") % reg % length << endl;
	break_in();
}
