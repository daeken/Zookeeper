#pragma once
#include "Kernel.hpp"

struct KINTERRUPT;

typedef BOOLEAN (NTAPI *PKSERVICE_ROUTINE)(KINTERRUPT *, PVOID);

enum KINTERRUPT_MODE {
	LevelSensitive = 0, 
	Latched = 1
};

typedef struct KINTERRUPT { // 0x70
	PKSERVICE_ROUTINE ServiceRoutine; // +0x0(0x4)
	PVOID ServiceContext; // +0x4(0x4)
	DWORD BusInterruptLevel; // +0x8(0x4)
	DWORD Irql; // +0xC(0x4)
	BYTE Connected; // +0x10(0x1)
	BYTE ShareVector; // +0x11(0x1)
	BYTE Mode; // +0x12(0x1)
	DWORD ServiceCount; // +0x14(0x4)
	DWORD DispatchCode[22]; // +0x18(0x58)
} KINTERRUPT;

uint32_t NTAPI kernel_HalGetInterruptVector(
	uint32_t BusInterruptLevel, 
	KIRQL *Irql
);

void NTAPI kernel_KeInitializeInterrupt(
	KINTERRUPT *Interrupt, 
	PKSERVICE_ROUTINE ServiceRoutine, 
	void *ServiceContext, 
	uint32_t Vector, 
	KIRQL Irql, 
	KINTERRUPT_MODE InterruptMode, 
	BOOLEAN ShareVector
);

BOOLEAN NTAPI kernel_KeConnectInterrupt(
	KINTERRUPT *Interrupt
);
