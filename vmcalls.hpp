#pragma once

typedef struct map_pages {
	uint32_t virt_base;
	uint32_t count;
} map_pages_t;

typedef struct unmap_pages {
	uint32_t virt_base;
	uint32_t count;
} unmap_pages_t;

#define VMCALL_LOG 1
#define VMCALL_MAP 2
#define VMCALL_UNMAP 3
