#pragma once
#include "Kernel.hpp"

void NTAPI kernel_KeInitializeTimerEx(
    IN PKTIMER      Timer,
    IN TIMER_TYPE   Type
);

BOOLEAN NTAPI kernel_KeSetTimer(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
);
