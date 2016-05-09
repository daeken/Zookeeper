#pragma once

#define OUT
#define IN
#define OPTIONAL

typedef int32_t NTSTATUS;
typedef uint32_t HANDLE;
typedef HANDLE *PHANDLE;

typedef uint32_t ULONG;
typedef ULONG *PULONG;
typedef bool BOOLEAN;
typedef void VOID;
typedef VOID *PVOID;

typedef VOID (NTAPI KSTART_ROUTINE)(IN PVOID StartContext);
typedef KSTART_ROUTINE *PKSTART_ROUTINE;
