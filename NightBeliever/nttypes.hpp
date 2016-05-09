#pragma once

#define OUT
#define IN
#define OPTIONAL

typedef int32_t NTSTATUS;
typedef uint32_t HANDLE;
typedef HANDLE *PHANDLE;

typedef uint8_t UCHAR;
typedef uint32_t ULONG;
typedef uint32_t DWORD;
typedef ULONG *PULONG;
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
