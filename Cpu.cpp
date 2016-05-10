#include "Zookeeper.hpp"

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

Cpu::Cpu(uint8_t *ram, uint8_t *kram) {
	mem = ram;
	kmem = kram;

	bailout(hv_vm_create(HV_VM_DEFAULT));
	memset(mem, 0, RAM_SIZE);
	bailout(hv_vm_map(mem, 0, RAM_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));
	bailout(hv_vm_map(kmem, 0xc0000000, KRAM_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));
	bailout(hv_vcpu_create(&vcpu, HV_VCPU_DEFAULT));

	uint64_t vmx_cap_pinbased, vmx_cap_procbased, vmx_cap_procbased2, vmx_cap_entry;
	bailout(hv_vmx_read_capability(HV_VMX_CAP_PINBASED, &vmx_cap_pinbased));
	bailout(hv_vmx_read_capability(HV_VMX_CAP_PROCBASED, &vmx_cap_procbased));
	bailout(hv_vmx_read_capability(HV_VMX_CAP_PROCBASED2, &vmx_cap_procbased2));
	bailout(hv_vmx_read_capability(HV_VMX_CAP_ENTRY, &vmx_cap_entry));

	wvmcs(VMCS_CTRL_PIN_BASED, cap2ctrl(vmx_cap_pinbased, 0));
	wvmcs(VMCS_CTRL_CPU_BASED, cap2ctrl(
		vmx_cap_procbased,
		VMCS_PRI_PROC_BASED_CTLS_HLT |
		VMCS_PRI_PROC_BASED_CTLS_CR8_LOAD |
		VMCS_PRI_PROC_BASED_CTLS_CR8_STORE));
	wvmcs(VMCS_CTRL_CPU_BASED2, cap2ctrl(vmx_cap_procbased2, 0));
	wvmcs(VMCS_CTRL_VMENTRY_CONTROLS, cap2ctrl(vmx_cap_entry, 0));
	wvmcs(VMCS_CTRL_EXC_BITMAP, 0xffffffff);
	wvmcs(VMCS_CTRL_CR0_MASK, 0xffffffff);
	wvmcs(VMCS_CTRL_CR0_SHADOW, 0xffffffff);
	wvmcs(VMCS_CTRL_CR4_MASK, 0xffffffff);
	wvmcs(VMCS_CTRL_CR4_SHADOW, 0xffffffff);

	uint32_t directory = 64*1024*1024; // Page directory base
	uint32_t *dir = (uint32_t *) (mem + directory);
	for(int i = 0; i < 1024; ++i) {
		dir[i] = (directory + 4096 + 4096 * i) | 0x7;
		uint32_t *table = (uint32_t *) (mem + (dir[i] & ~0xFFF));
		for(int j = 0; j < 1024; ++j) {
			table[j] = 0x0;
		}
	}
	wvmcs(VMCS_GUEST_CR3, directory);
	wvmcs(VMCS_GUEST_CR0, 0x80000000 | 0x20 | 0x01); // Paging | NE | PE

	uint8_t *gdt = ram + 96*1024*1024;
	memset(gdt, 0, 0x10000);
	gdt_encode(gdt, 0, 0, 0, 0); // Null entry
	gdt_encode(gdt, 1, 0, 0xffffffff, 0x9A); // Code
	gdt_encode(gdt, 2, 0, 0xffffffff, 0x92); // Data
	wvmcs(VMCS_GUEST_GDTR_LIMIT, 0xFFFF);
	wvmcs(VMCS_GUEST_GDTR_BASE, 96*1024*1024);
	map_pages(96 * 1024 * 1024, 96 * 1024 * 1024, 16);

	wvmcs(VMCS_GUEST_CS, 1 << 3);
	wvmcs(VMCS_GUEST_CS_AR, 0xc093);
	wvmcs(VMCS_GUEST_CS_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_CS_BASE, 0x0);

	wvmcs(VMCS_GUEST_DS, 2 << 3);
	wvmcs(VMCS_GUEST_DS_AR, 0xc093);
	wvmcs(VMCS_GUEST_DS_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_DS_BASE, 0);

	wvmcs(VMCS_GUEST_ES, 2 << 3);
	wvmcs(VMCS_GUEST_ES_AR, 0xc093);
	wvmcs(VMCS_GUEST_ES_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_ES_BASE, 0);

	wvmcs(VMCS_GUEST_FS, 2 << 3);
	wvmcs(VMCS_GUEST_FS_AR, 0xc093);
	wvmcs(VMCS_GUEST_FS_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_FS_BASE, 0);

	wvmcs(VMCS_GUEST_GS, 2 << 3);
	wvmcs(VMCS_GUEST_GS_AR, 0xc093);
	wvmcs(VMCS_GUEST_GS_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_GS_BASE, 0);

	wvmcs(VMCS_GUEST_SS, 2 << 3);
	wvmcs(VMCS_GUEST_SS_AR, 0xc093);
	wvmcs(VMCS_GUEST_SS_LIMIT, 0xffffffff);
	wvmcs(VMCS_GUEST_SS_BASE, 0);

	wvmcs(VMCS_GUEST_LDTR, 0);
	wvmcs(VMCS_GUEST_LDTR_LIMIT, 0);
	wvmcs(VMCS_GUEST_LDTR_AR, 0x10000);
	wvmcs(VMCS_GUEST_LDTR_BASE, 0);

	wvmcs(VMCS_GUEST_TR, 0);
	wvmcs(VMCS_GUEST_TR_LIMIT, 0);
	wvmcs(VMCS_GUEST_TR_AR, 0x83);
	wvmcs(VMCS_GUEST_TR_BASE, 0);

	wvmcs(VMCS_GUEST_CR4, 0x2000);
}

void Cpu::map_pages(uint32_t virt, uint32_t phys, uint32_t count) {
	uint32_t *dir = (uint32_t *) (mem + 64*1024*1024);
	for(int i = 0; i < count; ++i) {
		uint32_t *table = (uint32_t *) (mem + (dir[virt >> 22] & ~0xFFF));
		table[(virt >> 12) & 0x3ff] = phys | 0x7;
		virt += 4096;
		phys += 4096;
	}
	invalidate_tlb();
}

void Cpu::invalidate_tlb() {
	hv_vcpu_invalidate_tlb(vcpu);
}

uint32_t Cpu::virt2phys(uint32_t addr) {
	uint32_t cr3 = rreg(HV_X86_CR3);
	if(cr3 == 0)
		return addr;

	uint32_t *directory = (uint32_t *) (mem + cr3);
	uint32_t *table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
	return (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
}

bool Cpu::is_mapped(uint32_t addr) {
	uint32_t cr3 = rreg(HV_X86_CR3);
	if(cr3 == 0)
		return true;

	auto directory = (uint32_t *) (mem + cr3);
	auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
	return (table[(addr >> 12) & 0x3ff] & 1) == 1;
}

void Cpu::read_memory(uint32_t addr, uint32_t size, void *buffer) {
	auto cr3 = rreg(HV_X86_CR3);
	if(cr3 == 0) {
		if(addr >= 0xc0000000)
			memcpy(buffer, &kmem[addr - 0xc0000000], size);
		else
			memcpy(buffer, &mem[addr], size);
		return;
	}
	auto buf = (uint8_t *) buffer;
	auto directory = (uint32_t *) (mem + cr3);
	for(int i = 0; i < size; ++i) {
		auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
		auto paddr = (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
		if(paddr >= 0xc0000000)
			*(buf++) = kmem[paddr - 0xc0000000];
		else
			*(buf++) = mem[paddr];
		++addr;
	}
}

void Cpu::write_memory(uint32_t addr, uint32_t size, void *buffer) {
	uint32_t cr3 = rreg(HV_X86_CR3);
	if(cr3 == 0) {
		if(addr >= 0xc0000000)
			memcpy(&kmem[addr - 0xc0000000], buffer, size);
		else
			memcpy(&mem[addr], buffer, size);
		return;
	}
	uint8_t *buf = (uint8_t *) buffer;
	uint32_t *directory = (uint32_t *) (mem + cr3);
	for(int i = 0; i < size; ++i) {
		uint32_t *table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
		uint32_t paddr = (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
		if(paddr >= 0xc0000000)
			kmem[paddr - 0xc0000000] = *(buf++);
		else
			mem[paddr] = *(buf++);
		++addr;
	}
}

uint64_t Cpu::rdmsr(uint32_t msr) {
	switch(msr) {
		default:
			cout << "Unknown MSR being read: " << hex << msr << endl;
			return 0;
	}
}

void Cpu::wrmsr(uint32_t msr, uint64_t val) {
	switch(msr) {
		default:
			cout << "Unknown MSR being written: " << hex << msr << " == 0x" << hex << val << endl;
			break;
	}
}

Cpu::~Cpu() {
	bailout(hv_vcpu_destroy(vcpu));
	bailout(hv_vm_unmap(0, RAM_SIZE));
	bailout(hv_vm_destroy());
}

#define TASK_TIMER 20 // Milliseconds
#define TASK_INTERRUPT 80

uint64_t systime() {
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void log_exit_reason(uint32_t reason, uint32_t eip) {
	if(reason & 0x80000000) {
		reason &= 0x7FFFFFFF;
		cout << "Entry failed: ";
	} else
		cout << "Exit ";
	switch(reason) {
		case VMX_REASON_EXC_NMI:
			cout << "VMX_REASON_EXC_NMI";
			break;
		case VMX_REASON_IRQ:
			cout << "VMX_REASON_IRQ";
			break;
		case VMX_REASON_TRIPLE_FAULT:
			cout << "VMX_REASON_TRIPLE_FAULT";
			break;
		case VMX_REASON_INIT:
			cout << "VMX_REASON_INIT";
			break;
		case VMX_REASON_SIPI:
			cout << "VMX_REASON_SIPI";
			break;
		case VMX_REASON_IO_SMI:
			cout << "VMX_REASON_IO_SMI";
			break;
		case VMX_REASON_OTHER_SMI:
			cout << "VMX_REASON_OTHER_SMI";
			break;
		case VMX_REASON_IRQ_WND:
			cout << "VMX_REASON_IRQ_WND";
			break;
		case VMX_REASON_VIRTUAL_NMI_WND:
			cout << "VMX_REASON_VIRTUAL_NMI_WND";
			break;
		case VMX_REASON_TASK:
			cout << "VMX_REASON_TASK";
			break;
		case VMX_REASON_CPUID:
			cout << "VMX_REASON_CPUID";
			break;
		case VMX_REASON_GETSEC:
			cout << "VMX_REASON_GETSEC";
			break;
		case VMX_REASON_HLT:
			cout << "VMX_REASON_HLT";
			break;
		case VMX_REASON_INVD:
			cout << "VMX_REASON_INVD";
			break;
		case VMX_REASON_INVLPG:
			cout << "VMX_REASON_INVLPG";
			break;
		case VMX_REASON_RDPMC:
			cout << "VMX_REASON_RDPMC";
			break;
		case VMX_REASON_RDTSC:
			cout << "VMX_REASON_RDTSC";
			break;
		case VMX_REASON_RSM:
			cout << "VMX_REASON_RSM";
			break;
		case VMX_REASON_VMCALL:
			cout << "VMX_REASON_VMCALL";
			break;
		case VMX_REASON_VMCLEAR:
			cout << "VMX_REASON_VMCLEAR";
			break;
		case VMX_REASON_VMLAUNCH:
			cout << "VMX_REASON_VMLAUNCH";
			break;
		case VMX_REASON_VMPTRLD:
			cout << "VMX_REASON_VMPTRLD";
			break;
		case VMX_REASON_VMPTRST:
			cout << "VMX_REASON_VMPTRST";
			break;
		case VMX_REASON_VMREAD:
			cout << "VMX_REASON_VMREAD";
			break;
		case VMX_REASON_VMRESUME:
			cout << "VMX_REASON_VMRESUME";
			break;
		case VMX_REASON_VMWRITE:
			cout << "VMX_REASON_VMWRITE";
			break;
		case VMX_REASON_VMOFF:
			cout << "VMX_REASON_VMOFF";
			break;
		case VMX_REASON_VMON:
			cout << "VMX_REASON_VMON";
			break;
		case VMX_REASON_MOV_CR:
			cout << "VMX_REASON_MOV_CR";
			break;
		case VMX_REASON_MOV_DR:
			cout << "VMX_REASON_MOV_DR";
			break;
		case VMX_REASON_IO:
			cout << "VMX_REASON_IO";
			break;
		case VMX_REASON_RDMSR:
			cout << "VMX_REASON_RDMSR";
			break;
		case VMX_REASON_WRMSR:
			cout << "VMX_REASON_WRMSR";
			break;
		case VMX_REASON_VMENTRY_GUEST:
			cout << "VMX_REASON_VMENTRY_GUEST";
			break;
		case VMX_REASON_VMENTRY_MSR:
			cout << "VMX_REASON_VMENTRY_MSR";
			break;
		case VMX_REASON_MWAIT:
			cout << "VMX_REASON_MWAIT";
			break;
		case VMX_REASON_MTF:
			cout << "VMX_REASON_MTF";
			break;
		case VMX_REASON_MONITOR:
			cout << "VMX_REASON_MONITOR";
			break;
		case VMX_REASON_PAUSE:
			cout << "VMX_REASON_PAUSE";
			break;
		case VMX_REASON_VMENTRY_MC:
			cout << "VMX_REASON_VMENTRY_MC";
			break;
		case VMX_REASON_TPR_THRESHOLD:
			cout << "VMX_REASON_TPR_THRESHOLD";
			break;
		case VMX_REASON_APIC_ACCESS:
			cout << "VMX_REASON_APIC_ACCESS";
			break;
		case VMX_REASON_VIRTUALIZED_EOI:
			cout << "VMX_REASON_VIRTUALIZED_EOI";
			break;
		case VMX_REASON_GDTR_IDTR:
			cout << "VMX_REASON_GDTR_IDTR";
			break;
		case VMX_REASON_LDTR_TR:
			cout << "VMX_REASON_LDTR_TR";
			break;
		case VMX_REASON_EPT_VIOLATION:
			cout << "VMX_REASON_EPT_VIOLATION";
			break;
		case VMX_REASON_EPT_MISCONFIG:
			cout << "VMX_REASON_EPT_MISCONFIG";
			break;
		case VMX_REASON_EPT_INVEPT:
			cout << "VMX_REASON_EPT_INVEPT";
			break;
		case VMX_REASON_RDTSCP:
			cout << "VMX_REASON_RDTSCP";
			break;
		case VMX_REASON_VMX_TIMER_EXPIRED:
			cout << "VMX_REASON_VMX_TIMER_EXPIRED";
			break;
		case VMX_REASON_INVVPID:
			cout << "VMX_REASON_INVVPID";
			break;
		case VMX_REASON_WBINVD:
			cout << "VMX_REASON_WBINVD";
			break;
		case VMX_REASON_XSETBV:
			cout << "VMX_REASON_XSETBV";
			break;
		case VMX_REASON_APIC_WRITE:
			cout << "VMX_REASON_APIC_WRITE";
			break;
		case VMX_REASON_RDRAND:
			cout << "VMX_REASON_RDRAND";
			break;
		case VMX_REASON_INVPCID:
			cout << "VMX_REASON_INVPCID";
			break;
		case VMX_REASON_VMFUNC:
			cout << "VMX_REASON_VMFUNC";
			break;
		case VMX_REASON_RDSEED:
			cout << "VMX_REASON_RDSEED";
			break;
		case VMX_REASON_XSAVES:
			cout << "VMX_REASON_XSAVES";
			break;
		case VMX_REASON_XRSTORS:
			cout << "VMX_REASON_XRSTORS";
			break;
		default:
			cout << "Unknown reason: " << dec << reason;
	}
	cout << " @ " << hex << eip << endl;
}

void Cpu::run(uint32_t eip) {
	wreg(HV_X86_RIP, eip);
	wreg(HV_X86_RFLAGS, 0x2);

	box->debugger->enter(0);

	uint64_t last_time = systime();

	int stop = 0;
	do {
		bailout(hv_vcpu_run(vcpu));

		uint64_t exit_reason = rvmcs(VMCS_RO_EXIT_REASON);

		if(exit_reason & 0x80000000) {
			cout << "Entry failed? " << dec << (exit_reason & 0x7FFFFFFF) << endl;
			stop = 1;
		} else
			switch (exit_reason) {
				case VMX_REASON_EXC_NMI: {
					auto vec_val = rvmcs(VMCS_RO_VMEXIT_IRQ_INFO) & 0xFFFF;
					switch(vec_val >> 8) {
						case 6: { // Interrupt
							if((vec_val & 0xFF) == 3) {
								box->debugger->enter();
							} else {
								cout << "Unknown interrupt. " << dec << (vec_val & 0xFF) << endl;
								box->debugger->enter();
								exit(0);
							}
							break;
						}
						case 3: { // Exception
							switch(vec_val & 0xFF) {
								case 1: { // Single step
									auto flags = rreg(HV_X86_RFLAGS);
									wreg(HV_X86_RFLAGS, flags & ~(1 << 8));
									box->debugger->reenable();
									break;
								}
								case 6: {
									cout << "Invalid opcode" << endl;
									box->debugger->enter();
									break;
								}
								default:
									cout << "Unknown exception. " << dec << (vec_val & 0xFF) << endl;
									box->debugger->enter();
									exit(0);
							}
							break;
						}
						default:
							cout << "Unknown NMI class. " << hex << vec_val << endl;
					}
					break;
				}
				case VMX_REASON_VMCALL: {
					auto ret = vmcall_dispatch(rreg(HV_X86_RAX), rreg(HV_X86_RDX));
					wreg(HV_X86_RAX, ret);
					wreg(HV_X86_RIP, rreg(HV_X86_RIP) + 3);
					break;
				}
				case VMX_REASON_IRQ:
					break;
				case VMX_REASON_TRIPLE_FAULT:
					cout << "Triple fault!" << endl;
					cout << "IDTR Base " << hex << rreg(HV_X86_IDT_BASE) << " " << hex << rvmcs(VMCS_GUEST_IDTR_BASE) << endl;
					cout << "IDTR Limit " << hex << rreg(HV_X86_IDT_LIMIT) << endl;
					cout << "ISR 0 == " << hex << read_memory<uint64_t>(rvmcs(VMCS_GUEST_IDTR_BASE)) << endl;
					cout << "ISR 7 == " << hex << read_memory<uint64_t>(rvmcs(VMCS_GUEST_IDTR_BASE) + 8 * 7) << endl;
					stop = 1;
					break;
				case VMX_REASON_RDMSR: {
					uint64_t msr = 0;
					cout << "RDMSR " << hex << rreg(HV_X86_RCX);
					msr = rdmsr(rreg(HV_X86_RCX));
					cout << " == 0x" << hex << msr << endl;
					wreg(HV_X86_RIP, rreg(HV_X86_RIP) + 2);
					wreg(HV_X86_RAX, msr & 0xFFFFFFFF);
					wreg(HV_X86_RDX, msr >> 32);
					break;
				}
				case VMX_REASON_WRMSR: {
					uint64_t msr = ((uint64_t) rreg(HV_X86_RDX) << 32) | (uint64_t) rreg(HV_X86_RAX);
					cout << "WRMSR " << hex << rreg(HV_X86_RCX) << " == 0x" << hex << msr << endl;
					wrmsr(rreg(HV_X86_RCX), msr);
					wreg(HV_X86_RIP, rreg(HV_X86_RIP) + 2);
					break;
				}
				case VMX_REASON_HLT:
					cout << "HLT" << endl;
					stop = 1;
					break;
				case VMX_REASON_EPT_VIOLATION:
					// cout << "EPT_VIOLATION" << endl;
					break;
				default:
					cout << "Unhandled VM exit: " << dec << exit_reason << endl;
					stop = 1;
			}
		uint64_t cur_time = systime();
		if(cur_time >= last_time + TASK_TIMER) {
			box->tm->next();
			last_time = cur_time;
		}

		if(single_step) {
			single_step = false;
			auto flags = rreg(HV_X86_RFLAGS);
			wreg(HV_X86_RFLAGS, flags | (1 << 8));
		}
	} while(!stop);
	box->debugger->enter();
}
