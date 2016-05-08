#include "NightBeliever.hpp"

typedef struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr {
    uint16_t limit;
    idt_entry_t *base;
} __attribute__((packed)) idt_ptr_t;

static idt_ptr_t idtp;

void init_idt() {
	log("IDT initializing...");
	auto real_idt = new idt_entry_t[256];
	for(int i = 0; i < 256; ++i) {
		auto isr_addr = (uint32_t) all_isrs[i];
		real_idt[i].flags = 0x8E;
		real_idt[i].sel = 8;
		real_idt[i].always0 = 0;
		real_idt[i].base_hi = isr_addr >> 16;
		real_idt[i].base_lo = isr_addr & 0xFFFF;
	}

	idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
	idtp.base = real_idt;

	asm("lidt (_ZL4idtp)");
	asm("sti");

	log("Done.");
}

void handle_interrupt(isr_regs_t *regs) {
	switch(regs->int_no) {
		case 80: // TASK_INTERRUPT
			break;
		default:
			log("Unknown interrupt! %i @ 0x%08x", regs->int_no, regs->eip);
			halt();
			break;
	}
}
