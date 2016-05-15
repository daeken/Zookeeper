#include "NightBeliever.hpp"

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
	log("StartContext1 %08x", s.StartContext1);
	log("StartContext2 %08x", s.StartContext2);

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

uint32_t kernel_LaunchDataPage = 0;
uint32_t kernel_IdexChannelObject = 0;
XBOX_HARDWARE_INFO kernel_XboxHardwareInfo = {
	0xC0000035,
	0,0,0,0
};
uint32_t kernel_XboxKrnlVersion = 0;

void kernel_DbgPrint(char *format, ...) {
	va_list arglist;

	va_start(arglist, format);
	log("DbgPrint:");
	log(format, arglist);
	va_end(arglist);
}

void NTAPI kernel_HalReturnToFirmware() {
	log("HalReturnToFirmware");
	halt();
}

void NTAPI kernel_RtlAssert(char *message, char *filename, uint32_t line, uint32_t unk) {
	if(message == NULL) message = (char *) "~NULL~";
	if(filename == NULL) filename = (char *) "~NULL~";
	log("Failed assert %s in %s on line %i (unknown %i == 0x%x)", message, filename, line, unk, unk);
	log("Around address 0x%08x", ((uint32_t *) &message)[-1]);
	halt();
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
	log("RtlEnterCriticalSection");
	kernel_RtlInitializeCriticalSection(crit); // XBEs don't seem to do this.

	return 0;
}

void NTAPI kernel_RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *crit) {
	log("RtlLeaveCriticalSection");
}

void NTAPI kernel_HalRegisterShutdownNotification(
	PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration, 
	CHAR Register
) {
	log("HalRegisterShutdownNotification");
}

void NTAPI kernel_KeInitializeDpc(
	KDPC                *Dpc,
	PKDEFERRED_ROUTINE   DeferredRoutine,
	PVOID                DeferredContext
) {
	log("KeInitializeDPC");

	Dpc->Number = 0;
	Dpc->DeferredRoutine = DeferredRoutine;
	Dpc->Type = DpcObject;
	Dpc->DeferredContext = DeferredContext;
}

void NTAPI kernel_KeInitializeTimerEx(
	IN PKTIMER      Timer,
	IN TIMER_TYPE   Type
) {
	Timer->Header.Type               = Type + 8;
	Timer->Header.Inserted           = 0;
	Timer->Header.Size               = sizeof(*Timer) / sizeof(ULONG);
	Timer->Header.SignalState        = 0;
	Timer->TimerListEntry.Blink      = NULL;
	Timer->TimerListEntry.Flink      = NULL;
	Timer->Header.WaitListHead.Flink = &Timer->Header.WaitListHead;
	Timer->Header.WaitListHead.Blink = &Timer->Header.WaitListHead;
	Timer->DueTime.QuadPart          = 0;
	Timer->Period                    = 0;
}

NTSTATUS NTAPI kernel_ExQueryNonVolatileSetting(
	IN  DWORD               ValueIndex,
	OUT DWORD              *Type,
	OUT PUCHAR              Value,
	IN  SIZE_T              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
) {
	auto val = query_eeprom(ValueIndex);
	if(Type)
		*Type = 4;
	if(Value)
		*((uint32_t *) Value) = val;
	if(ResultLength)
		*ResultLength = 4;

	return STATUS_SUCCESS;
}

ULONG NTAPI kernel_RtlNtStatusToDosError(NTSTATUS Status) {
	return 0;
}

BOOLEAN NTAPI kernel_KeSetTimer(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC          Dpc OPTIONAL
) {
	return true;
}

NTSTATUS NTAPI kernel_NtAllocateVirtualMemory(
	void **BaseAddress,
	uint32_t *ZeroBits, 
	size_t *RegionSize, 
	uint32_t AllocationType, 
	uint32_t Protect
) {
	log(
		"NtAllocateVirtualMemory(0x%08x, %i, 0x%08x, 0x%08x, 0x%08x)", 
		*BaseAddress, (ZeroBits != NULL) ? *ZeroBits : -1, *RegionSize, AllocationType, Protect
	);
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

	log("Allocated memory at 0x%08x", *BaseAddress);

	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtOpenFile(
	HANDLE *FileHandle, 
	ACCESS_MASK DesiredAccess, 
	OBJECT_ATTRIBUTES *ObjectAttributes, 
	IO_STATUS_BLOCK *IoStatusBlock, 
	uint32_t ShareAccess, 
	uint32_t OpenOptions
) {
	*FileHandle = io_open(ObjectAttributes->RootDirectory, (char *) ObjectAttributes->ObjectName->Buffer);
	if(*FileHandle == 0)
		return STATUS_OBJECT_NAME_NOT_FOUND;
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtClose(HANDLE handle) {
	log("NtClose %08x", handle);
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtQueryVolumeInformationFile(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PVOID                       FileInformation,
	IN  ULONG                       Length,
	IN  FS_INFORMATION_CLASS        FileInformationClass
) {
	if(IoStatusBlock)
		debug("Ignoring status block");
	
	switch(FileInformationClass) {
		case FileFsSizeInformation: {
			log("FsSizeInformation");
			auto info = (FILE_FS_SIZE_INFORMATION *) FileInformation;
			// Test XBE *requires* bytes per allocation unit == 16kb!
			info->BytesPerSector = 4096; // 4KB sectors
			info->SectorsPerAllocationUnit = 4; // 16KB allocation units
			info->TotalAllocationUnits.QuadPart = 524288; // 8GB drive
			info->AvailableAllocationUnits.QuadPart = 458752; // 7GB free
			break;
		}
		default:
			bailout("Unknown FS_INFORMATION_CLASS %i", FileInformationClass);
	}
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_IoCreateSymbolicLink(
	IN PSTRING SymbolicLinkName,
	IN PSTRING DeviceName
) {
	log("IoCreateSymbolicLink('%s', '%s')", SymbolicLinkName->Buffer, DeviceName->Buffer);
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtCreateFile(
	OUT PHANDLE             FileHandle, 
	IN  ACCESS_MASK         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  PLARGE_INTEGER      AllocationSize, 
	IN  ULONG               FileAttributes, 
	IN  ULONG               ShareAccess, 
	IN  ULONG               CreateDisposition, 
	IN  ULONG               CreateOptions 
) {
	log("NtCreateFile('%s')", ObjectAttributes->ObjectName->Buffer);
	return STATUS_SUCCESS;
}
