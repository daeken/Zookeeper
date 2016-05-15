#include "Kernel.hpp"

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

BOOLEAN NTAPI kernel_KeSetTimer(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC          Dpc OPTIONAL
) {
	return true;
}
