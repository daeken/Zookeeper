#pragma once
#include "Kernel.hpp"

extern uint32_t kernel_LaunchDataPage;
extern uint32_t kernel_IdexChannelObject;
extern XBOX_HARDWARE_INFO kernel_XboxHardwareInfo;
extern uint32_t kernel_XboxKrnlVersion;

void kernel_DbgPrint(char *format, ...);
void NTAPI kernel_RtlAssert(char *message, char *filename, uint32_t line, uint32_t unk);

void NTAPI kernel_HalReturnToFirmware();
void NTAPI kernel_HalRegisterShutdownNotification(
    PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration, 
    CHAR Register
);

NTSTATUS NTAPI kernel_ExQueryNonVolatileSetting(
    IN  DWORD               ValueIndex,
    OUT DWORD              *Type,
    OUT PUCHAR              Value,
    IN  SIZE_T              ValueLength,
    OUT PSIZE_T             ResultLength OPTIONAL
);

ULONG NTAPI kernel_RtlNtStatusToDosError(NTSTATUS Status);
