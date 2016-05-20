#include "Kernel.hpp"

uint32_t NTAPI kernel_HalGetInterruptVector(
	uint32_t BusInterruptLevel, 
	KIRQL *Irql
) {
	// XXX: Implement
	log("HalGetInterruptVector(0x%08x, 0x%08x)", BusInterruptLevel, Irql);
	return 0xDEADBEEF;
}

void NTAPI kernel_KeInitializeInterrupt(
	KINTERRUPT *Interrupt, 
	PKSERVICE_ROUTINE ServiceRoutine, 
	void *ServiceContext, 
	uint32_t Vector, 
	KIRQL Irql, 
	KINTERRUPT_MODE InterruptMode, 
	BOOLEAN ShareVector
) {
	// XXX: Implement
	log("KeInitializeInterrupt(0x%08x)", Interrupt);
}

BOOLEAN NTAPI kernel_KeConnectInterrupt(
	KINTERRUPT *Interrupt
) {
	// XXX: Implement
	log("KeConnectInterrupt(0x%08x)", Interrupt);
	return true;
}
