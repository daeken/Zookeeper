#pragma once

#include "Kernel.hpp"

void NTAPI kernel_HalReadWritePCISpace(
	uint32_t BusNumber, 
	uint32_t SlotNumber, 
	uint32_t RegisterNumber, 
	void *Buffer, 
	uint32_t Length, 
	BOOLEAN WritePCISpace
);
