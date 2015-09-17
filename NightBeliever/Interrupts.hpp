#pragma once
#include "NightBeliever.hpp"

typedef struct isr_regs {
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
} isr_regs_t;

extern void *all_isrs[256];

void init_idt();
void handle_interrupt(isr_regs_t *regs);
