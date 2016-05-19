#pragma once
#include "Kernel.hpp"

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
    IN ULONG   NumberOfBytes,
    IN BOOLEAN Persist
);
PVOID NTAPI kernel_MmAllocateContiguousMemory(IN ULONG NumberOfBytes);

NTSTATUS NTAPI kernel_NtAllocateVirtualMemory(
    void **BaseAddress,
    uint32_t *ZeroBits, 
    size_t *RegionSize, 
    uint32_t AllocationType, 
    uint32_t Protect
);

NTSTATUS NTAPI kernel_NtFreeVirtualMemory(
	void **BaseAddress,
	uint32_t *FreeSize,
	uint32_t FreeType
);
