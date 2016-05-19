#pragma once

#define IOCTL_DISK_GET_DRIVE_GEOMETRY 0x70000
#define IOCTL_DISK_GET_PARTITION_INFO 0x74004

typedef struct __attribute__((packed)) DISK_GEOMETRY {
	uint64_t Cylinders; // 0x00
	uint32_t MediaType, TracksPerCylinder, SectorsPerTrack, BytesPerSector; // 0x08 0x0C 0x10 0x14
} DISK_GEOMETRY;

typedef struct __attribute__((packed)) PARTITION_INFORMATION {
	uint64_t StartingOffset, PartitionLength; // 0x00 0x08
	uint32_t HiddenSectors, PartitionNumber; // 0x10 0x14
	uint8_t PartitionType, BootIndicator, RecognizedPartition, RewritePartition; // 0x18 0x19 0x20 0x21
} PARTITION_INFORMATION;
