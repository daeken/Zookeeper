// These types owe a ton to CXBX and Wine!

#pragma once

#define OUT
#define IN
#define OPTIONAL

typedef int32_t NTSTATUS;
typedef uint32_t HANDLE;
typedef HANDLE *PHANDLE;

typedef int8_t CHAR;
typedef int8_t *PCHAR;
typedef uint8_t UCHAR;
typedef uint8_t *PUCHAR;
typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef uint32_t DWORD;
typedef uint32_t SIZE_T;
typedef uint32_t *PSIZE_T;
typedef ULONG *PULONG;
typedef int64_t LONGLONG;
typedef uint64_t ULONGLONG;
typedef bool BOOLEAN;
typedef void VOID;
typedef VOID *PVOID;

typedef VOID (NTAPI KSTART_ROUTINE)(IN PVOID StartContext);
typedef KSTART_ROUTINE *PKSTART_ROUTINE;

#define NT_SUCCESS(Status)              ((NTSTATUS) (Status) >= 0)
#define STATUS_SUCCESS                   ((DWORD   )0x00000000L)  
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_TIMER_RESUME_IGNORED      ((DWORD   )0x40000025L)  
#define STATUS_UNSUCCESSFUL              ((DWORD   )0xC0000001)  
#define STATUS_UNRECOGNIZED_MEDIA        ((DWORD   )0xC0000014)  
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ALERTED                   ((DWORD   )0x00000101)  
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
// The SCSI input buffer was too large (not necessarily an error!)
#define STATUS_DATA_OVERRUN              ((DWORD   )0xC000003CL)  
#define STATUS_INVALID_IMAGE_FORMAT      ((DWORD   )0xC000007BL)  
#define STATUS_INSUFFICIENT_RESOURCES    ((DWORD   )0xC000009AL)  
#define STATUS_TOO_MANY_SECRETS          ((DWORD   )0xC0000156L)  
#define STATUS_XBE_REGION_MISMATCH       ((DWORD   )0xC0050001L)  
#define STATUS_XBE_MEDIA_MISMATCH        ((DWORD   )0xC0050002L)  
#define STATUS_OBJECT_NAME_NOT_FOUND     ((DWORD   )0xC0000034L)
#define STATUS_OBJECT_NAME_COLLISION     ((DWORD   )0xC0000035L)

typedef struct _NT_TIB {
	PVOID                                   ExceptionList;          // 0x00
	PVOID                                   StackBase;              // 0x04
	PVOID                                   StackLimit;             // 0x08
	PVOID                                   SubSystemTib;           // 0x0C
	union {
		PVOID FiberData;                                            // 0x10 for TIB
		ULONG Version;                                              // 0x10 for TEB (?)
	} u_a;
	PVOID                                   ArbitraryUserPointer;   // 0x14
	struct _NT_TIB                         *Self;                   // 0x18
} NT_TIB, *PNT_TIB;

typedef struct _KTHREAD {
	UCHAR           UnknownA[0x28];
	PVOID           TlsData;        // 0x28
	UCHAR           UnknownB[0xE4]; // 0x2C
} KTHREAD, *PKTHREAD;

typedef struct _ETHREAD {
	struct _KTHREAD Tcb;
	UCHAR           UnknownA[0x1C]; // 0x110
	DWORD           UniqueThread;   // 0x12C
} ETHREAD, *PETHREAD;

typedef struct _KPRCB {
	struct _KTHREAD* CurrentThread;                                 // 0x00, KPCR : 0x28
	struct _KTHREAD* NextThread;                                    // 0x04, KPCR : 0x2C
	struct _KTHREAD* IdleThread;                                    // 0x08, KPCR : 0x30

	// This is the total size of the structure (presumably)
	UCHAR            Unknown[0x250];                                // 0x0C, KPCR : 0x34
} KPRCB, *PKPRCB;

typedef struct _KPCR {
	struct _NT_TIB  NtTib;                                          // 0x00
	struct _KPCR   *SelfPcr;                                        // 0x1C
	struct _KPRCB  *Prcb;                                           // 0x20
	UCHAR           Irql;                                           // 0x24
	struct _KPRCB   PrcbData;                                       // 0x28
} KPCR, *PKPCR;

typedef struct _RTL_CRITICAL_SECTION {
	DWORD               Unknown[4];                                     // 0x00
	LONG                LockCount;                                      // 0x10
	LONG                RecursionCount;                                 // 0x14
	ULONG               OwningThread;                                   // 0x18
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef struct _XBOX_HARDWARE_INFO {
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
	struct {
		DWORD LowPart;
		DWORD HighPart;
	} u1;

	struct {
		DWORD LowPart;
		DWORD HighPart;
	} u;

	ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef union _LARGE_INTEGER {
	struct {
		DWORD   LowPart;
		LONG    HighPart;
	} u;

	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _DISPATCHER_HEADER {
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

#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define MEM_RELEASE 0x8000
#define MEM_FREE 0x10000
#define MEM_PRIVATE 0x20000
#define MEM_MAPPED 0x40000
#define MEM_RESET 0x80000
#define MEM_TOP_DOWN 0x100000

typedef uint32_t ACCESS_MASK;

typedef struct _STRING {
	USHORT  Length;
	USHORT  MaximumLength;
	PCHAR   Buffer;
} STRING, ANSI_STRING, *PSTRING, *PANSI_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	HANDLE  RootDirectory;
	PSTRING ObjectName;
	ULONG   Attributes;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	} u1;

	ULONG *Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _FSINFOCLASS {
	FileFsVolumeInformation       = 1,
	FileFsLabelInformation,      // 2
	FileFsSizeInformation,       // 3
	FileFsDeviceInformation,     // 4
	FileFsAttributeInformation,  // 5
	FileFsControlInformation,    // 6
	FileFsFullSizeInformation,   // 7
	FileFsObjectIdInformation,   // 8
	FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_FS_SIZE_INFORMATION {
	LARGE_INTEGER   TotalAllocationUnits; // 0
	LARGE_INTEGER   AvailableAllocationUnits; // 08h
	ULONG           SectorsPerAllocationUnit; // 10h
	ULONG           BytesPerSector; // 14h
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;
