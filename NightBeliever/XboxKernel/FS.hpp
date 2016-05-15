#pragma once
#include "Kernel.hpp"

NTSTATUS NTAPI kernel_NtOpenFile(
    HANDLE *FileHandle, 
    ACCESS_MASK DesiredAccess, 
    OBJECT_ATTRIBUTES *ObjectAttributes, 
    IO_STATUS_BLOCK *IoStatusBlock, 
    uint32_t ShareAccess, 
    uint32_t OpenOptions
);

NTSTATUS NTAPI kernel_NtClose(HANDLE handle);

NTSTATUS NTAPI kernel_NtQueryVolumeInformationFile(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PVOID                       FileInformation,
    IN  ULONG                       Length,
    IN  FS_INFORMATION_CLASS        FileInformationClass
);

NTSTATUS NTAPI kernel_IoCreateSymbolicLink(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
);

NTSTATUS NTAPI kernel_NtCreateFile(
    OUT PHANDLE             FileHandle, 
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize, 
    IN  ULONG               FileAttributes, 
    IN  ULONG               ShareAccess, 
    IN  ULONG               CreateDisposition, 
    IN  ULONG               CreateOptions 
);
