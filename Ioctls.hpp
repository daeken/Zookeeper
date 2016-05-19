#pragma once

#define IOCTL_DISK_GET_PARTITION_INFO 0x74004

typedef struct __attribute__((packed)) PARTITION_INFORMATION {
	uint64_t StartingOffset, PartitionLength;
	uint32_t HiddenSectors, PartitionNumber;
	uint8_t PartitionType, BootIndicator, RecognizedPartition, RewritePartition;
} PARTITION_INFORMATION;
