import glob, re, sys

imports = '''AvGetSavedDataAddress 80000001
AvSendTVEncoderOption 80000002
AvSetDisplayMode 80000003
AvSetSavedDataAddress 80000004
DbgBreakPoint 80000005
DbgBreakPointWithStatus 80000006
DbgLoadImageSymbols 80000007
DbgPrint 80000008
HalReadSMCTrayState 80000009
DbgPrompt 8000000A
DbgUnLoadImageSymbols 8000000B
ExAcquireReadWriteLockExclusive 8000000C
ExAcquireReadWriteLockShared 8000000D
ExAllocatePool 8000000E
ExAllocatePoolWithTag 8000000F
ExEventObjectType 80000010
ExFreePool 80000011
ExInitializeReadWriteLock 80000012
ExInterlockedAddLargeInteger 80000013
ExInterlockedAddLargeStatistic 80000014
ExInterlockedCompareExchange64 80000015
ExMutantObjectType 80000016
ExQueryPoolBlockSize 80000017
ExQueryNonVolatileSetting 80000018
ExReadWriteRefurbInfo 80000019
ExRaiseException 8000001A
ExRaiseStatus 8000001B
ExReleaseReadWriteLock 8000001C
ExSaveNonVolatileSetting 8000001D
ExSemaphoreObjectType 8000001E
ExTimerObjectType 8000001F
ExfInterlockedInsertHeadList 80000020
ExfInterlockedInsertTailList 80000021
ExfInterlockedRemoveHeadList 80000022
FscGetCacheSize 80000023
FscInvalidateIdleBlocks 80000024
FscSetCacheSize 80000025
HalClearSoftwareInterrupt 80000026
HalDisableSystemInterrupt 80000027
HalDiskCachePartitionCount 80000028
HalDiskModelNumber 80000029
HalDiskSerialNumber 8000002A
HalEnableSystemInterrupt 8000002B
HalGetInterruptVector 8000002C
HalReadSMBusValue 8000002D
HalReadWritePCISpace 8000002E
HalRegisterShutdownNotification 8000002F
HalRequestSoftwareInterrupt 80000030
HalReturnToFirmware 80000031
HalWriteSMBusValue 80000032
InterlockedCompareExchange 80000033
InterlockedDecrement 80000034
InterlockedIncrement 80000035
InterlockedExchange 80000036
InterlockedExchangeAdd 80000037
InterlockedFlushSList 80000038
InterlockedPopEntrySList 80000039
InterlockedPushEntrySList 8000003A
IoAllocateIrp 8000003B
IoBuildAsynchronousFsdRequest 8000003C
IoBuildDeviceIoControlRequest 8000003D
IoBuildSynchronousFsdRequest 8000003E
IoCheckShareAccess 8000003F
IoCompletionObjectType 80000040
IoCreateDevice 80000041
IoCreateFile 80000042
IoCreateSymbolicLink 80000043
IoDeleteDevice 80000044
IoDeleteSymbolicLink 80000045
IoDeviceObjectType 80000046
IoFileObjectType 80000047
IoFreeIrp 80000048
IoInitializeIrp 80000049
IoInvalidDeviceRequest 8000004A
IoQueryFileInformation 8000004B
IoQueryVolumeInformation 8000004C
IoQueueThreadIrp 8000004D
IoRemoveShareAccess 8000004E
IoSetIoCompletion 8000004F
IoSetShareAccess 80000050
IoStartNextPacket 80000051
IoStartNextPacketByKey 80000052
IoStartPacket 80000053
IoSynchronousDeviceIoControlRequest 80000054
IoSynchronousFsdRequest 80000055
IofCallDriver 80000056
IofCompleteRequest 80000057
KdDebuggerEnabled 80000058
KdDebuggerNotPresent 80000059
IoDismountVolume 8000005A
IoDismountVolumeByName 8000005B
KeAlertResumeThread 8000005C
KeAlertThread 8000005D
KeBoostPriorityThread 8000005E
KeBugCheck 8000005F
KeBugCheckEx 80000060
KeCancelTimer 80000061
KeConnectInterrupt 80000062
KeDelayExecutionThread 80000063
KeDisconnectInterrupt 80000064
KeEnterCriticalRegion 80000065
MmGlobalData 80000066
KeGetCurrentIrql 80000067
KeGetCurrentThread 80000068
KeInitializeApc 80000069
KeInitializeDeviceQueue 8000006A
KeInitializeDpc 8000006B
KeInitializeEvent 8000006C
KeInitializeInterrupt 8000006D
KeInitializeMutant 8000006E
KeInitializeQueue 8000006F
KeInitializeSemaphore 80000070
KeInitializeTimerEx 80000071
KeInsertByKeyDeviceQueue 80000072
KeInsertDeviceQueue 80000073
KeInsertHeadQueue 80000074
KeInsertQueue 80000075
KeInsertQueueApc 80000076
KeInsertQueueDpc 80000077
KeInterruptTime 80000078
KeIsExecutingDpc 80000079
KeLeaveCriticalRegion 8000007A
KePulseEvent 8000007B
KeQueryBasePriorityThread 8000007C
KeQueryInterruptTime 8000007D
KeQueryPerformanceCounter 8000007E
KeQueryPerformanceFrequency 8000007F
KeQuerySystemTime 80000080
KeRaiseIrqlToDpcLevel 80000081
KeRaiseIrqlToSynchLevel 80000082
KeReleaseMutant 80000083
KeReleaseSemaphore 80000084
KeRemoveByKeyDeviceQueue 80000085
KeRemoveDeviceQueue 80000086
KeRemoveEntryDeviceQueue 80000087
KeRemoveQueue 80000088
KeRemoveQueueDpc 80000089
KeResetEvent 8000008A
KeRestoreFloatingPointState 8000008B
KeResumeThread 8000008C
KeRundownQueue 8000008D
KeSaveFloatingPointState 8000008E
KeSetBasePriorityThread 8000008F
KeSetDisableBoostThread 80000090
KeSetEvent 80000091
KeSetEventBoostPriority 80000092
KeSetPriorityProcess 80000093
KeSetPriorityThread 80000094
KeSetTimer 80000095
KeSetTimerEx 80000096
KeStallExecutionProcessor 80000097
KeSuspendThread 80000098
KeSynchronizeExecution 80000099
KeSystemTime 8000009A
KeTestAlertThread 8000009B
KeTickCount 8000009C
KeTimeIncrement 8000009D
KeWaitForMultipleObjects 8000009E
KeWaitForSingleObject 8000009F
KfRaiseIrql 800000A0
KfLowerIrql 800000A1
KiBugCheckData 800000A2
KiUnlockDispatcherDatabase 800000A3
LaunchDataPage 800000A4
MmAllocateContiguousMemory 800000A5
MmAllocateContiguousMemoryEx 800000A6
MmAllocateSystemMemory 800000A7
MmClaimGpuInstanceMemory 800000A8
MmCreateKernelStack 800000A9
MmDeleteKernelStack 800000AA
MmFreeContiguousMemory 800000AB
MmFreeSystemMemory 800000AC
MmGetPhysicalAddress 800000AD
MmIsAddressValid 800000AE
MmLockUnlockBufferPages 800000AF
MmLockUnlockPhysicalPage 800000B0
MmMapIoSpace 800000B1
MmPersistContiguousMemory 800000B2
MmQueryAddressProtect 800000B3
MmQueryAllocationSize 800000B4
MmQueryStatistics 800000B5
MmSetAddressProtect 800000B6
MmUnmapIoSpace 800000B7
NtAllocateVirtualMemory 800000B8
NtCancelTimer 800000B9
NtClearEvent 800000BA
NtClose 800000BB
NtCreateDirectoryObject 800000BC
NtCreateEvent 800000BD
NtCreateFile 800000BE
NtCreateIoCompletion 800000BF
NtCreateMutant 800000C0
NtCreateSemaphore 800000C1
NtCreateTimer 800000C2
NtDeleteFile 800000C3
NtDeviceIoControlFile 800000C4
NtDuplicateObject 800000C5
NtFlushBuffersFile 800000C6
NtFreeVirtualMemory 800000C7
NtFsControlFile 800000C8
NtOpenDirectoryObject 800000C9
NtOpenFile 800000CA
NtOpenSymbolicLinkObject 800000CB
NtProtectVirtualMemory 800000CC
NtPulseEvent 800000CD
NtQueueApcThread 800000CE
NtQueryDirectoryFile 800000CF
NtQueryDirectoryObject 800000D0
NtQueryEvent 800000D1
NtQueryFullAttributesFile 800000D2
NtQueryInformationFile 800000D3
NtQueryIoCompletion 800000D4
NtQueryMutant 800000D5
NtQuerySemaphore 800000D6
NtQuerySymbolicLinkObject 800000D7
NtQueryTimer 800000D8
NtQueryVirtualMemory 800000D9
NtQueryVolumeInformationFile 800000DA
NtReadFile 800000DB
NtReadFileScatter 800000DC
NtReleaseMutant 800000DD
NtReleaseSemaphore 800000DE
NtRemoveIoCompletion 800000DF
NtResumeThread 800000E0
NtSetEvent 800000E1
NtSetInformationFile 800000E2
NtSetIoCompletion 800000E3
NtSetSystemTime 800000E4
NtSetTimerEx 800000E5
NtSignalAndWaitForSingleObjectEx 800000E6
NtSuspendThread 800000E7
NtUserIoApcDispatcher 800000E8
NtWaitForSingleObject 800000E9
NtWaitForSingleObjectEx 800000EA
NtWaitForMultipleObjectsEx 800000EB
NtWriteFile 800000EC
NtWriteFileGather 800000ED
NtYieldExecution 800000EE
ObCreateObject 800000EF
ObDirectoryObjectType 800000F0
ObInsertObject 800000F1
ObMakeTemporaryObject 800000F2
ObOpenObjectByName 800000F3
ObOpenObjectByPointer 800000F4
ObpObjectHandleTable 800000F5
ObReferenceObjectByHandle 800000F6
ObReferenceObjectByName 800000F7
ObReferenceObjectByPointer 800000F8
ObSymbolicLinkObjectType 800000F9
ObfDereferenceObject 800000FA
ObfReferenceObject 800000FB
PhyGetLinkState 800000FC
PhyInitialize 800000FD
PsCreateSystemThread 800000FE
PsCreateSystemThreadEx 800000FF
PsQueryStatistics 80000100
PsSetCreateThreadNotifyRoutine 80000101
PsTerminateSystemThread 80000102
PsThreadObjectType 80000103
RtlAnsiStringToUnicodeString 80000104
RtlAppendStringToString 80000105
RtlAppendUnicodeStringToString 80000106
RtlAppendUnicodeToString 80000107
RtlAssert 80000108
RtlCaptureContext 80000109
RtlCaptureStackBackTrace 8000010A
RtlCharToInteger 8000010B
RtlCompareMemory 8000010C
RtlCompareMemoryUlong 8000010D
RtlCompareString 8000010E
RtlCompareUnicodeString 8000010F
RtlCopyString 80000110
RtlCopyUnicodeString 80000111
RtlCreateUnicodeString 80000112
RtlDowncaseUnicodeChar 80000113
RtlDowncaseUnicodeString 80000114
RtlEnterCriticalSection 80000115
RtlEnterCriticalSectionAndRegion 80000116
RtlEqualString 80000117
RtlEqualUnicodeString 80000118
RtlExtendedIntegerMultiply 80000119
RtlExtendedLargeIntegerDivide 8000011A
RtlExtendedMagicDivide 8000011B
RtlFillMemory 8000011C
RtlFillMemoryUlong 8000011D
RtlFreeAnsiString 8000011E
RtlFreeUnicodeString 8000011F
RtlGetCallersAddress 80000120
RtlInitAnsiString 80000121
RtlInitUnicodeString 80000122
RtlInitializeCriticalSection 80000123
RtlIntegerToChar 80000124
RtlIntegerToUnicodeString 80000125
RtlLeaveCriticalSection 80000126
RtlLeaveCriticalSectionAndRegion 80000127
RtlLowerChar 80000128
RtlMapGenericMask 80000129
RtlMoveMemory 8000012A
RtlMultiByteToUnicodeN 8000012B
RtlMultiByteToUnicodeSize 8000012C
RtlNtStatusToDosError 8000012D
RtlRaiseException 8000012E
RtlRaiseStatus 8000012F
RtlTimeFieldsToTime 80000130
RtlTimeToTimeFields 80000131
RtlTryEnterCriticalSection 80000132
RtlUlongByteSwap 80000133
RtlUnicodeStringToAnsiString 80000134
RtlUnicodeStringToInteger 80000135
RtlUnicodeToMultiByteN 80000136
RtlUnicodeToMultiByteSize 80000137
RtlUnwind 80000138
RtlUpcaseUnicodeChar 80000139
RtlUpcaseUnicodeString 8000013A
RtlUpcaseUnicodeToMultiByteN 8000013B
RtlUpperChar 8000013C
RtlUpperString 8000013D
RtlUshortByteSwap 8000013E
RtlWalkFrameChain 8000013F
RtlZeroMemory 80000140
XboxEEPROMKey 80000141
XboxHardwareInfo 80000142
XboxHDKey 80000143
XboxKrnlVersion 80000144
XboxSignatureKey 80000145
XeImageFileName 80000146
XeLoadSection 80000147
XeUnloadSection 80000148
READ_PORT_BUFFER_UCHAR 80000149
READ_PORT_BUFFER_USHORT 8000014A
READ_PORT_BUFFER_ULONG 8000014B
WRITE_PORT_BUFFER_UCHAR 8000014C
WRITE_PORT_BUFFER_USHORT 8000014D
WRITE_PORT_BUFFER_ULONG 8000014E
XcSHAInit 8000014F
XcSHAUpdate 80000150
XcSHAFinal 80000151
XcRC4Key 80000152
XcRC4Crypt 80000153
XcHMAC 80000154
XcPKEncPublic 80000155
XcPKDecPrivate 80000156
XcPKGetKeyLen 80000157
XcVerifyPKCS1Signature 80000158
XcModExp 80000159
XcDESKeyParity 8000015A
XcKeyTable 8000015B
XcBlockCrypt 8000015C
XcBlockCryptCBC 8000015D
XcCryptService 8000015E
XcUpdateCrypto 8000015F
RtlRip 80000160
XboxLANKey 80000161
XboxAlternateSignatureKeys 80000162
XePublicKeyData 80000163
HalBootSMCVideoMode 80000164
IdexChannelObject 80000165
HalIsResetOrShutdownPending 80000166
IoMarkIrpMustComplete 80000167
HalInitiateShutdown 80000168
snprintf 80000169
sprintf 8000016A
vsnprintf 8000016B
vsprintf 8000016C
HalEnableSecureTrayEject 8000016D
HalWriteSMCScratchRegister 8000016E'''.split('\n')

rh = ''
for fn in glob.glob('XboxKernel/*.hpp'):
	if fn != 'XboxKernel/KernelThunk.hpp':
		rh += file(('' if len(sys.argv) == 1 else sys.argv[1] + '/') + fn, 'r').read() + '\n'
def defined(sym):
	if re.search(r'kernel_' + sym + r'[;\s(]', rh, re.M | re.S):
		return True
	return False

ks = file('XboxKernel/KernelThunk.cpp', 'w')
print >>ks, '// Automatically generated by thunkgen.py'
print >>ks, '#include "NightBeliever.hpp"'
print >>ks

kh = file('XboxKernel/KernelThunk.hpp', 'w')
print >>kh, '// Automatically generated by thunkgen.py'
print >>kh, '#pragma once'
print >>kh, '#include "NightBeliever.hpp"'
print >>kh

total = 0
stubbed = 0
for line in imports:
	name, id = line.split(' ')
	id = int(id, 16)
	if not defined(name):
		print >>ks, 'void NTAPI kernel_%s() {' % name
		print >>ks, '\tlog("STUB %s");' % name
		print >>ks, '\thalt();'
		print >>ks, '}'
		print >>ks

		print >>kh, 'void NTAPI kernel_%s();' % name
		stubbed += 1
	total += 1
print >>kh

print '%i/%i kernel exports implemented' % (total - stubbed, total)

print >>kh, 'uint32_t thunk_lookup(uint32_t id);'
print >>ks, 'uint32_t thunk_lookup(uint32_t id) {'
print >>ks, '\tswitch(id) {'
for line in imports:
	name, id = line.split(' ')
	id = int(id, 16)

	print >>ks, '\t\tcase 0x%08x: return (uint32_t) &kernel_%s;' % (id, name)

print >>ks, '\t\tdefault:'
print >>ks, '\t\t\tlog("Unknown id to thunk_lookup %08x", id);'
print >>ks, '\t\t\thalt();'
print >>ks, '\t\t\treturn 0;'

print >>ks, '\t}'
print >>ks, '}'
