#include "Kernel.hpp"

void NTAPI kernel_HalReadWritePCISpace(
	uint32_t BusNumber, 
	uint32_t SlotNumber, 
	uint32_t RegisterNumber, 
	void *Buffer, 
	uint32_t Length, 
	BOOLEAN WritePCISpace
) {
	if(WritePCISpace)
		pci_write(BusNumber, SlotNumber, RegisterNumber, Buffer, Length);
	else
		pci_read(BusNumber, SlotNumber, RegisterNumber, Buffer, Length);
}
