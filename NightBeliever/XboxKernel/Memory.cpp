#include "Kernel.hpp"

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
	IN ULONG   NumberOfBytes,
	IN BOOLEAN Persist
) {
	//log("Ignore MmPersistContiguousMemory");
}

void * NTAPI kernel_MmAllocateContiguousMemory(uint32_t NumberOfBytes) {
	return kernel_MmAllocateContiguousMemoryEx(NumberOfBytes, 0, 0xFFFFFFFF, 0, 0);
}

void * NTAPI kernel_MmAllocateContiguousMemoryEx(
	uint32_t NumberOfBytes, 
	uint32_t low, uint32_t high, 
	uint32_t unk, uint32_t flags
) {
	NumberOfBytes = pagepad(NumberOfBytes);
	return map_contiguous(0, low, high, NumberOfBytes / 4096);
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
		*BaseAddress = map(*BaseAddress, *RegionSize / 4096);
	} else if((AllocationType & MEM_RESERVE) == MEM_RESERVE) {
		// We should just be reserving memory, but it doesn't matter.
		// Commit will trash this region and we're probably leaking some.
		// Future coders will deal with this problem.
		*BaseAddress = map(*BaseAddress, *RegionSize / 4096);
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

uint32_t NTAPI kernel_MmQueryAllocationSize(void *base) {
	// XXX: This won't return the size of a given allocation,
	// but how many pages are mapped after that point.
	// This might blow up sometime.
	return query_map_size(base);
}

uint32_t NTAPI kernel_MmQueryAddressProtect(void *base) {
	// XXX: Implement
	return 0xFFFFFFFF;
}

uint32_t NTAPI kernel_MmClaimGpuInstanceMemory(
    uint32_t NumberOfBytes, 
    uint32_t *NumberOfPaddingBytes
) {
	// XXX: Implement properly
	log("MmClaimGpuInstanceMemory(0x%08x)", NumberOfBytes);
	*NumberOfPaddingBytes = 0;
	return NumberOfBytes;
}
