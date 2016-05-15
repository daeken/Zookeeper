#pragma once
#include "Kernel.hpp"

void NTAPI kernel_RtlInitAnsiString(
	STRING *DestinationString,
	char *SourceString
);

bool NTAPI kernel_RtlEqualString(
	STRING *String1, 
	STRING *String2, 
	bool CaseInsensitive
);
