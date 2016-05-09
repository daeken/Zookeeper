#include "NightBeliever.hpp"

void gdt_encode(uint8_t *gdt, int entry, uint32_t base, uint32_t limit, uint8_t type) {
	gdt += 8 * entry;
	if(limit > 65536) {
		limit >>= 12;
		gdt[6] = 0xc0;
	} else
		gdt[6] = 0x40;

    gdt[0] = limit & 0xFF;
    gdt[1] = (limit >> 8) & 0xFF;
    gdt[6] |= (limit >> 16) & 0xF;
 
    gdt[2] = base & 0xFF;
    gdt[3] = (base >> 8) & 0xFF;
    gdt[4] = (base >> 16) & 0xFF;
    gdt[7] = (base >> 24) & 0xFF;
 
    gdt[5] = type;
}

void init_tib() {
	auto gdt = (uint8_t *) (96 * 1024 * 1024);
	auto entry = -1;
	for(auto i = 3 * 8; i < 8192 * 8; i += 8) {
		if((gdt[i + 6] & 0x80) == 0) {
			entry = i / 8;
			break;
		}
	}

	if(entry == -1) {
		log("Ran out of GDT entries!");
		halt();
	}

	auto tib = new uint32_t[0x10000];
	tib[0x20 / 4] = (uint32_t) tib;
	gdt_encode(gdt, entry, (uint32_t) tib, 0xFFFFFFFF, 0x92);
	asm("mov %0, %%fs" :: "r"(entry << 3));
}