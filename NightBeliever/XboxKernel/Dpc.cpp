#include "Kernel.hpp"

void NTAPI kernel_KeInitializeDpc(
	KDPC                *Dpc,
	PKDEFERRED_ROUTINE   DeferredRoutine,
	PVOID                DeferredContext
) {
	Dpc->Number = 0;
	Dpc->DeferredRoutine = DeferredRoutine;
	Dpc->Type = DpcObject;
	Dpc->DeferredContext = DeferredContext;
}
