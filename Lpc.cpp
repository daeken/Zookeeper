#include "Zookeeper.hpp"

// Comment to enable break_in()s for this file
#define break_in() do { } while(0)

Lpc::Lpc() : Device("Lpc") {
	box->add_pci(1, 0, this);
}

void Lpc::readPci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Lpc::readPci(0x%08x, %i)") % reg % length << endl;
	break_in();
}

void Lpc::writePci(uint32_t reg, void *buffer, uint32_t length) {
	cout << format("Lpc::writePci(0x%08x, %i)") % reg % length << endl;
	break_in();
}
