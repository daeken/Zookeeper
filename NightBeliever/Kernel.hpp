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
