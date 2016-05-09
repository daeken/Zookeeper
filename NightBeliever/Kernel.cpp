#include "NightBeliever.hpp"

typedef struct threadexp {
	PKSTART_ROUTINE StartRoutine;
	PVOID StartContext1, StartContext2;
} threadexp_t;

void threadex_proxy(uint32_t up) {
	auto p = (threadexp_t *) up;
	auto s = *p;
	delete p;

	init_tib();

	asm(
		"mov %0, %%esi\n"
		"push %1\n"
		"push %2\n"
		"push callComplete\n"
		"lea 4(%%esp), %%ebp\n"
		"jmp *%%esi\n"
		"callComplete:"
		:: "r"(s.StartRoutine), "r"(s.StartContext2), "r"(s.StartContext1)
	);

	log("Thread complete");
	halt();
}

NTSTATUS NTAPI kernel_PsCreateSystemThreadEx(
	OUT PHANDLE         ThreadHandle,
    IN  ULONG           ThreadExtraSize,
    IN  ULONG           KernelStackSize,
    IN  ULONG           TlsDataSize,
    OUT PULONG          ThreadId OPTIONAL,
    IN  PVOID           StartContext1,
    IN  PVOID           StartContext2,
    IN  BOOLEAN         CreateSuspended,
    IN  BOOLEAN         DebugStack,
    IN  PKSTART_ROUTINE StartRoutine
) {
	if(CreateSuspended) {
		log("Suspended threads not supported.");
		halt();
	}

	auto p = new threadexp_t;
	p->StartRoutine = StartRoutine;
	p->StartContext1 = StartContext1;
	p->StartContext2 = StartContext2;

	create_thread(threadex_proxy, malloc(1024*1024) + 1024 * 1024, (uint32_t) p);

	return 0;
}

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
    IN ULONG   NumberOfBytes,
    IN BOOLEAN Persist
) {
	log("Ignore MmPersistContiguousMemory");
}

PVOID NTAPI kernel_MmAllocateContiguousMemory(IN ULONG NumberOfBytes) {
	return (PVOID) new uint8_t[NumberOfBytes];
}

uint32_t kernel_LaunchDataPage = 0;
uint32_t kernel_IdexChannelObject = 0;

void kernel_DbgPrint(char *format, ...) {
	va_list arglist;

	va_start(arglist, format);
	log("DbgPrint:");
	log(format, arglist);
	va_end(arglist);
}