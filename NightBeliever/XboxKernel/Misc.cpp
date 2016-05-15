#include "Kernel.hpp"

uint32_t kernel_LaunchDataPage = 0;
uint32_t kernel_IdexChannelObject = 0;
XBOX_HARDWARE_INFO kernel_XboxHardwareInfo = {
	0xC0000035,
	0,0,0,0
};
uint32_t kernel_XboxKrnlVersion = 0;

void kernel_DbgPrint(char *format, ...) {
	va_list arglist;

	va_start(arglist, format);
	log("DbgPrint:");
	log(format, arglist);
	va_end(arglist);
}

void NTAPI kernel_HalReturnToFirmware() {
	log("HalReturnToFirmware");
	halt();
}

void NTAPI kernel_RtlAssert(char *message, char *filename, uint32_t line, uint32_t unk) {
	if(message == NULL) message = (char *) "~NULL~";
	if(filename == NULL) filename = (char *) "~NULL~";
	log("Failed assert %s in %s on line %i (unknown %i == 0x%x)", message, filename, line, unk, unk);
	log("Around address 0x%08x", ((uint32_t *) &message)[-1]);
	halt();
}

void NTAPI kernel_HalRegisterShutdownNotification(
	PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration, 
	CHAR Register
) {
	log("HalRegisterShutdownNotification");
}

NTSTATUS NTAPI kernel_ExQueryNonVolatileSetting(
	IN  DWORD               ValueIndex,
	OUT DWORD              *Type,
	OUT PUCHAR              Value,
	IN  SIZE_T              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
) {
	auto val = query_eeprom(ValueIndex);
	if(Type)
		*Type = 4;
	if(Value)
		*((uint32_t *) Value) = val;
	if(ResultLength)
		*ResultLength = 4;

	return STATUS_SUCCESS;
}

ULONG NTAPI kernel_RtlNtStatusToDosError(NTSTATUS Status) {
	return 0;
}
