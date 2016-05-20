#pragma once
#include "Kernel.hpp"

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
    IN ULONG   NumberOfBytes,
    IN BOOLEAN Persist
);
void * NTAPI kernel_MmAllocateContiguousMemory(uint32_t NumberOfBytes);
void * NTAPI kernel_MmAllocateContiguousMemoryEx(
    uint32_t NumberOfBytes, 
    uint32_t low, uint32_t high, 
    uint32_t unk, uint32_t flags
);

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

uint32_t NTAPI kernel_MmQueryAllocationSize(void *base);
uint32_t NTAPI kernel_MmQueryAddressProtect(void *base);

uint32_t NTAPI kernel_MmClaimGpuInstanceMemory(
    uint32_t NumberOfBytes, 
    uint32_t *NumberOfPaddingBytes
);
