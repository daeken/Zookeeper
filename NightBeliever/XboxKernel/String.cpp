#include "Kernel.hpp"

void NTAPI kernel_RtlInitAnsiString(
	STRING *DestinationString,
	char *SourceString
) {
	auto len = DestinationString->Length = DestinationString->MaximumLength = strlen(SourceString);
	DestinationString->Buffer = new CHAR[len + 1];
	memcpy(DestinationString->Buffer, SourceString, len + 1);
}

bool NTAPI kernel_RtlEqualString(
	STRING *String1, 
	STRING *String2, 
	bool CaseInsensitive
) {
	if(CaseInsensitive)
		return stricmp((char *) String1->Buffer, (char *) String2->Buffer) == 0;
	else
		return strcmp((char *) String1->Buffer, (char *) String2->Buffer) == 0;
}
