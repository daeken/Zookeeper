#pragma once
#include "Zookeeper.hpp"

enum HVReg {
	EIP, 
	EFLAGS, 
	EAX, ECX, EDX, EBX, 
	ESI, EDI, ESP, EBP, 
	CS, SS, DS, ES, FS, GS, 
	IDT_BASE, IDT_LIMIT, 
	GDT_BASE, GDT_LIMIT, 
	LDTR, LDT_BASE, LDT_LIMIT, LDT_AR, 
	TR, TSS_BASE, TSS_LIMIT, TSS_AR, 
	CR0, CR1, CR2, CR3, CR4, 
	DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7, 
	TPR, XCR0
};

enum HVExitReason {
	Ignore, 
	Unknown, 
	EntryFailed, 
	Interrupt, 
	Exception, 
	VmCall, 
	PortIO, 
	TripleFault, 
	Hlt
};

typedef struct exit {
	HVExitReason reason;
	uint32_t error_code;
	uint32_t instruction_length;
	uint32_t interrupt_no;

	// Used for page faults
	uint32_t address;

	// Used for port IO
	uint32_t port;
	uint32_t port_size; // 8, 16, or 32 bits
	bool port_direction; // true is OUT, false is IN
} exit_t;

class HV {
public:
	virtual ~HV() = 0;
	virtual void map_phys(void *memory, uint32_t base, uint32_t size) = 0;
	virtual void invalidate_tlb() = 0;
	virtual uint32_t reg(HVReg reg) = 0;
	virtual void reg(HVReg reg, uint32_t val) = 0;
	virtual exit_t enter() = 0;
};
