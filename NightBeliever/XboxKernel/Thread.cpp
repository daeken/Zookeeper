#include "Kernel.hpp"

typedef struct threadexp {
	PKSTART_ROUTINE StartRoutine;
	PVOID StartContext1, StartContext2;
} threadexp_t;

void threadex_proxy(uint32_t tid, uint32_t up) {
	auto p = (threadexp_t *) up;
	auto s = *p;
	delete p;

	init_tib(tid);

	log("Starting xbthread at %08x", s.StartRoutine);

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

	log("Thread %i complete", tid);
	terminate_thread();
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

	*ThreadHandle = create_thread(threadex_proxy, ((uint8_t *) malloc(1024*1024)) + 1024 * 1024, (uint32_t) p);
	if(ThreadId != NULL)
		*ThreadId = *ThreadHandle;

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_RtlInitializeCriticalSection(RTL_CRITICAL_SECTION *crit) {
	// A hack to ensure we haven't already initialized this
	if(crit->Unknown[0] == 0xDEADBEEF)
		return 0;
	crit->Unknown[0] = 0xDEADBEEF;
	crit->LockCount = -1;
	crit->RecursionCount = 0;
	crit->OwningThread = get_thread_id();

	return 0;
}

NTSTATUS NTAPI kernel_RtlEnterCriticalSection(RTL_CRITICAL_SECTION *crit) {
	// XXX: Implement
	kernel_RtlInitializeCriticalSection(crit); // XBEs don't seem to do this.

	return 0;
}

void NTAPI kernel_RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *crit) {
	// XXX: Implement
}
