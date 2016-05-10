#pragma once

#define OUT
#define IN
#define OPTIONAL

typedef int32_t NTSTATUS;
typedef uint32_t HANDLE;
typedef HANDLE *PHANDLE;

typedef int8_t CHAR;
typedef uint8_t UCHAR;
typedef uint8_t *PUCHAR;
typedef int16_t SHORT;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef uint32_t DWORD;
typedef uint32_t SIZE_T;
typedef uint32_t *PSIZE_T;
typedef ULONG *PULONG;
typedef uint64_t ULONGLONG;
typedef bool BOOLEAN;
typedef void VOID;
typedef VOID *PVOID;

typedef VOID (NTAPI KSTART_ROUTINE)(IN PVOID StartContext);
typedef KSTART_ROUTINE *PKSTART_ROUTINE;

typedef struct _NT_TIB
{
	PVOID                                   ExceptionList;          // 0x00
	PVOID                                   StackBase;              // 0x04
	PVOID                                   StackLimit;             // 0x08
	PVOID                                   SubSystemTib;           // 0x0C
	union
	{
		PVOID FiberData;                                            // 0x10 for TIB
		ULONG Version;                                              // 0x10 for TEB (?)
	}
	u_a;
	PVOID                                   ArbitraryUserPointer;   // 0x14
	struct _NT_TIB                         *Self;                   // 0x18
} NT_TIB, *PNT_TIB;

typedef struct _KTHREAD
{
	UCHAR           UnknownA[0x28];
	PVOID           TlsData;        // 0x28
	UCHAR           UnknownB[0xE4]; // 0x2C
} KTHREAD, *PKTHREAD;

typedef struct _ETHREAD
{
	struct _KTHREAD Tcb;
	UCHAR           UnknownA[0x1C]; // 0x110
	DWORD           UniqueThread;   // 0x12C
} ETHREAD, *PETHREAD;

typedef struct _KPRCB
{
	struct _KTHREAD* CurrentThread;                                 // 0x00, KPCR : 0x28
	struct _KTHREAD* NextThread;                                    // 0x04, KPCR : 0x2C
	struct _KTHREAD* IdleThread;                                    // 0x08, KPCR : 0x30

	// This is the total size of the structure (presumably)
	UCHAR            Unknown[0x250];                                // 0x0C, KPCR : 0x34
} KPRCB, *PKPRCB;

typedef struct _KPCR
{
	struct _NT_TIB  NtTib;                                          // 0x00
	struct _KPCR   *SelfPcr;                                        // 0x1C
	struct _KPRCB  *Prcb;                                           // 0x20
	UCHAR           Irql;                                           // 0x24
	struct _KPRCB   PrcbData;                                       // 0x28
} KPCR, *PKPCR;

typedef struct _RTL_CRITICAL_SECTION
{
	DWORD               Unknown[4];                                     // 0x00
	LONG                LockCount;                                      // 0x10
	LONG                RecursionCount;                                 // 0x14
	ULONG               OwningThread;                                   // 0x18
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef struct _XBOX_HARDWARE_INFO
{
	ULONG Flags;
	UCHAR Unknown1;
	UCHAR Unknown2;
	UCHAR Unknown3;
	UCHAR Unknown4;
} XBOX_HARDWARE_INFO;

typedef struct _LIST_ENTRY {
  struct _LIST_ENTRY  *Flink;
  struct _LIST_ENTRY  *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef struct HAL_SHUTDOWN_REGISTRATION { // 0x10
	PVOID NotificationRoutine; // +0x0(0x4)
	DWORD Priority; // +0x4(0x4)
	LIST_ENTRY ListEntry; // +0x8(0x8)
} *PHAL_SHUTDOWN_REGISTRATION;

typedef VOID (*PKDEFERRED_ROUTINE)(
	IN struct _KDPC *Dpc,
	IN PVOID         DeferredContext,
	IN PVOID         SystemArgument1,
	IN PVOID         SystemArgument2
);

typedef struct _KDPC {
	SHORT               Type;               // 0x00
	UCHAR               Number;             // 0x02
	UCHAR               Importance;         // 0x03
	LIST_ENTRY          DpcListEntry;       // 0x04
	PKDEFERRED_ROUTINE  DeferredRoutine;    // 0x0C
	PVOID               DeferredContext;
	PVOID               SystemArgument1;
	PVOID               SystemArgument2;
} KDPC, *PKDPC;

typedef enum _KOBJECTS {
	DpcObject = 0x13,
} KOBJECTS, *PKOBJECTS;

typedef union _ULARGE_INTEGER {
	struct
	{
		DWORD LowPart;
		DWORD HighPart;
	}
	u1;

	struct
	{
		DWORD LowPart;
		DWORD HighPart;
	}
	u;

	ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct _DISPATCHER_HEADER
{
	UCHAR       Type;           // 0x00
	UCHAR       Absolute;       // 0x01
	UCHAR       Size;           // 0x02
	UCHAR       Inserted;       // 0x03
	LONG        SignalState;    // 0x04
	LIST_ENTRY  WaitListHead;   // 0x08
} DISPATCHER_HEADER;

typedef struct _KTIMER {
	DISPATCHER_HEADER   Header;           // 0x00
	ULARGE_INTEGER      DueTime;          // 0x10
	LIST_ENTRY          TimerListEntry;   // 0x18
	struct _KDPC       *Dpc;              // 0x20
	LONG                Period;           // 0x24
} KTIMER, *PKTIMER;

typedef enum _TIMER_TYPE {
	NotificationTimer     = 0,
	SynchronizationTimer  = 1
} TIMER_TYPE;
