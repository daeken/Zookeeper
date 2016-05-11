#pragma once
#include "NightBeliever.hpp"

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
);

void NTAPI kernel_MmPersistContiguousMemory(
	IN PVOID   BaseAddress,
    IN ULONG   NumberOfBytes,
    IN BOOLEAN Persist
);
PVOID NTAPI kernel_MmAllocateContiguousMemory(IN ULONG NumberOfBytes);

extern uint32_t kernel_LaunchDataPage;
extern uint32_t kernel_IdexChannelObject;
extern XBOX_HARDWARE_INFO kernel_XboxHardwareInfo;
extern uint32_t kernel_XboxKrnlVersion;

void kernel_DbgPrint(char *format, ...);
void NTAPI kernel_HalReturnToFirmware();

void NTAPI kernel_RtlAssert(char *message, char *filename, uint32_t line, uint32_t unk);

NTSTATUS NTAPI kernel_RtlInitializeCriticalSection(RTL_CRITICAL_SECTION *crit);
NTSTATUS NTAPI kernel_RtlEnterCriticalSection(RTL_CRITICAL_SECTION *crit);
void NTAPI kernel_RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *crit);

void NTAPI kernel_HalRegisterShutdownNotification(
    PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration, 
    CHAR Register
);

void NTAPI kernel_KeInitializeDpc(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
);

void NTAPI kernel_KeInitializeTimerEx(
    IN PKTIMER      Timer,
    IN TIMER_TYPE   Type
);

NTSTATUS NTAPI kernel_ExQueryNonVolatileSetting(
    IN  DWORD               ValueIndex,
    OUT DWORD              *Type,
    OUT PUCHAR              Value,
    IN  SIZE_T              ValueLength,
    OUT PSIZE_T             ResultLength OPTIONAL
);

ULONG NTAPI kernel_RtlNtStatusToDosError(NTSTATUS Status);

BOOLEAN NTAPI kernel_KeSetTimer(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
);

NTSTATUS NTAPI kernel_NtAllocateVirtualMemory(
    void **BaseAddress,
    uint32_t *ZeroBits, 
    size_t *RegionSize, 
    uint32_t AllocationType, 
    uint32_t Protect
);

NTSTATUS NTAPI kernel_NtOpenFile(
    HANDLE *FileHandle, 
    ACCESS_MASK DesiredAccess, 
    OBJECT_ATTRIBUTES *ObjectAttributes, 
    IO_STATUS_BLOCK *IoStatusBlock, 
    uint32_t ShareAccess, 
    uint32_t OpenOptions
);
