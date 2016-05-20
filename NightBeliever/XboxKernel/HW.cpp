#include "Kernel.hpp"

void NTAPI kernel_HalReadWritePCISpace(
	uint32_t BusNumber, 
	uint32_t SlotNumber, 
	uint32_t RegisterNumber, 
	void *Buffer, 
	uint32_t Length, 
	BOOLEAN WritePCISpace
) {
	log("HalReadWritePCISpace(%i, %i, %i, 0x%08x, %i, %i)", BusNumber, SlotNumber, RegisterNumber, Buffer, Length, WritePCISpace);
	if(WritePCISpace)
		pci_write(BusNumber, SlotNumber, RegisterNumber, Buffer, Length);
	else
		pci_read(BusNumber, SlotNumber, RegisterNumber, Buffer, Length);
}
