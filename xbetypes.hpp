#pragma once

typedef struct XbeHeader {
	uint32_t magic;
	uint8_t signature[256];
	uint32_t base, soh, soi, soih;
	uint32_t timedate, certaddr, numsects, secthdrs, flags;
	uint32_t oep, tls;
	uint32_t stack_commit, heap_reserve, heap_commit, pe_base;
	uint32_t pe_soi, pe_csum, pe_timedate;
	uint32_t debug_pathname, debug_filename, debug_ufilename;
	uint32_t thunk, imports, numvers, libvers, kvers, xapivers;
	uint32_t logoaddr, logosize;
} XbeHeader_t;

typedef struct XbeSection {
	uint32_t flags, vaddr, vsize, raddr, rsize;
	uint32_t nameaddr, nameref, headref, tailref;
	uint8_t digest[20];
} XbeSection_t;