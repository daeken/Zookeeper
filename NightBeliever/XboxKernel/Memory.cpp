#include "Kernel.hpp"

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
	IN ULONG   NumberOfBytes,
	IN BOOLEAN Persist
) {
	//log("Ignore MmPersistContiguousMemory");
}

PVOID NTAPI kernel_MmAllocateContiguousMemory(IN ULONG NumberOfBytes) {
	return (PVOID) new uint8_t[NumberOfBytes];
}

NTSTATUS NTAPI kernel_NtAllocateVirtualMemory(
	void **BaseAddress,
	uint32_t *ZeroBits, 
	size_t *RegionSize, 
	uint32_t AllocationType, 
	uint32_t Protect
) {
	*BaseAddress = (void *) (((uint32_t) *BaseAddress) & ~0xFFF);
	*RegionSize = pagepad(*RegionSize);
	if((AllocationType & MEM_COMMIT) == MEM_COMMIT) {
		*BaseAddress = map_aligned(*BaseAddress, *RegionSize / 4096);
	} else if((AllocationType & MEM_RESERVE) == MEM_RESERVE) {
		// We should just be reserving memory, but it doesn't matter.
		// Commit will trash this region and we're probably leaking some.
		// Future coders will deal with this problem.
		*BaseAddress = map_aligned(*BaseAddress, *RegionSize / 4096);
	} else {
		bailout("Unsupported allocation type %x", AllocationType);
	}

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtFreeVirtualMemory(
	void **BaseAddress,
	uint32_t *FreeSize,
	uint32_t FreeType
) {
	*BaseAddress = (void *) (((uint32_t) *BaseAddress) & ~0xFFF);
	*FreeSize = pagepad(*FreeSize);

	unmap(*BaseAddress, *FreeSize / 4096);
	
	return STATUS_SUCCESS;
}
