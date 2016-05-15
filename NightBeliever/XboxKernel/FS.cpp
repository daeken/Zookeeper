#include "Kernel.hpp"

NTSTATUS NTAPI kernel_NtOpenFile(
	HANDLE *FileHandle, 
	ACCESS_MASK DesiredAccess, 
	OBJECT_ATTRIBUTES *ObjectAttributes, 
	IO_STATUS_BLOCK *IoStatusBlock, 
	uint32_t ShareAccess, 
	uint32_t OpenOptions
) {
	log("NtOpenFile('%s')", ObjectAttributes->ObjectName->Buffer);
	*FileHandle = io_open(ObjectAttributes->RootDirectory, (char *) ObjectAttributes->ObjectName->Buffer);
	if(*FileHandle == 0)
		return STATUS_OBJECT_NAME_NOT_FOUND;
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtClose(HANDLE handle) {
	log("NtClose %08x", handle);
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_NtQueryVolumeInformationFile(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PVOID                       FileInformation,
	IN  ULONG                       Length,
	IN  FS_INFORMATION_CLASS        FileInformationClass
) {
	if(IoStatusBlock)
		debug("Ignoring status block");
	
	switch(FileInformationClass) {
		case FileFsSizeInformation: {
			log("FsSizeInformation");
			auto info = (FILE_FS_SIZE_INFORMATION *) FileInformation;
			// Test XBE *requires* bytes per allocation unit == 16kb!
			info->BytesPerSector = 4096; // 4KB sectors
			info->SectorsPerAllocationUnit = 4; // 16KB allocation units
			info->TotalAllocationUnits.QuadPart = 524288; // 8GB drive
			info->AvailableAllocationUnits.QuadPart = 458752; // 7GB free
			break;
		}
		default:
			bailout("Unknown FS_INFORMATION_CLASS %i", FileInformationClass);
	}
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI kernel_IoCreateSymbolicLink(
	IN PSTRING SymbolicLinkName,
	IN PSTRING DeviceName
) {
	log("IoCreateSymbolicLink('%s', '%s')", SymbolicLinkName->Buffer, DeviceName->Buffer);
	return STATUS_SUCCESS;
}

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
) {
	log("NtCreateFile('%s')", ObjectAttributes->ObjectName->Buffer);
	*FileHandle = io_open(ObjectAttributes->RootDirectory, (char *) ObjectAttributes->ObjectName->Buffer);
	if(*FileHandle == 0) {
		return STATUS_OBJECT_NAME_NOT_FOUND;
	}
	return STATUS_SUCCESS;
}
