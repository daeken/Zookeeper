#pragma once
#include "Kernel.hpp"

void NTAPI kernel_KeInitializeDpc(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
);
