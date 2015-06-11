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

Cpu::Cpu(uint8_t *ram, Kernel *_kernel) {
	kernel = _kernel;
	mem = ram;

	bailout(hv_vm_create(HV_VM_DEFAULT));
	bailout(hv_vm_map(mem, 0, RAM_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));
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
	wvmcs(VMCS_CTRL_CR4_MASK, 0);
	wvmcs(VMCS_CTRL_CR4_SHADOW, 0);

	uint32_t directory = 32*1024*1024; // Page directory base
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

	uint8_t *gdt = ram + 0x10000;
	gdt_encode(gdt, 0, 0, 0, 0); // Null entry
	gdt_encode(gdt, 1, 0, 0xffffffff, 0x9A); // Code
	gdt_encode(gdt, 2, 0, 0xffffffff, 0x92); // Data
	wvmcs(VMCS_GUEST_GDTR_LIMIT, 0x100);
	wvmcs(VMCS_GUEST_GDTR_BASE, 0x10000);

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

	uint8_t *idt = ram + 0x16000;
	for(int i = 0; i < 256; ++i) {
		idt[0] = idt[1] = idt[6] = idt[7] = 0xFF;
		idt[2] = 1;
		idt[3] = 0;
		idt[4] = 0;
		idt[5] = 0x8E;
	}
	wvmcs(VMCS_GUEST_IDTR_LIMIT, 256 * 8);
	wvmcs(VMCS_GUEST_IDTR_BASE, 0x16000);

	wvmcs(VMCS_GUEST_CR4, 0x2000);
}

void Cpu::map_pages(uint32_t virt, uint32_t phys, uint32_t count) {
	uint32_t *dir = (uint32_t *) (mem + 32*1024*1024);
	for(int i = 0; i < count; ++i) {
		uint32_t *table = (uint32_t *) (mem + (dir[virt >> 22] & ~0xFFF));
		table[(virt >> 12) & 0x3ff] = phys | 0x7;
		virt += 4096;
		phys += 4096;
	}
	hv_vcpu_invalidate_tlb(vcpu);
}

void Cpu::alloc_stack(uint32_t bottom_virt, uint32_t bottom_phys, uint32_t size) {
	wreg(HV_X86_RSP, bottom_virt + size);
	wreg(HV_X86_RBP, bottom_virt + size - 0x20);
	map_pages(bottom_virt, bottom_phys, size / 4096);
}

void Cpu::read_memory(uint32_t addr, uint32_t size, void *buffer) {
	uint8_t *buf = (uint8_t *) buffer;
	uint32_t *directory = (uint32_t *) (mem + rreg(HV_X86_CR3));
	for(int i = 0; i < size; ++i) {
		uint32_t *table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
		*(buf++) = mem[(table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF)];
		++addr;
	}
}

template<typename T> T Cpu::read_memory(uint32_t addr) {
	T value = 0;
	read_memory(addr, sizeof(T), &value);
	return value;
}

Cpu::~Cpu() {
	bailout(hv_vcpu_destroy(vcpu));
	bailout(hv_vm_unmap(0, RAM_SIZE));
	bailout(hv_vm_destroy());
}

void Cpu::run(uint32_t eip) {
	wreg(HV_X86_RIP, eip);
	wreg(HV_X86_RFLAGS, 0x2);

	int stop = 0;
	do {
		cout << "Running from EIP: " << hex << rreg(HV_X86_RIP) << endl;
		bailout(hv_vcpu_run(vcpu));
		uint64_t exit_reason = rvmcs(VMCS_RO_EXIT_REASON);

		switch (exit_reason) {
			case VMX_REASON_EXC_NMI: {
				uint64_t vec_val = rvmcs(VMCS_RO_IDT_VECTOR_INFO) & 0xFFFF;
				stop = 1;
				cout << "VMX_REASON_EXC_NMI " << hex << (vec_val & 0xFF) << endl;
				if(vec_val >> 8 == 6)
					cout << "Exception." << endl;
				else if(vec_val >> 8 == 4) {
					cout << "Software interrupt." << endl;
					if((vec_val & 0xFF) == 0x69) { // Kernel thunk!
						uint32_t func = read_memory<uint32_t>(rreg(HV_X86_RSP));
						wreg(HV_X86_RSP, rreg(HV_X86_RSP) + 4);
						bailout(kernel->dispatch_call(func));
						stop = 0;
					}
				}
				else if(vec_val >> 8 == 2)
					cout << "NMI." << endl;
				else if(vec_val == 0)
					cout << "!!!Out of bounds access!!!" << endl;
				else
					cout << "WTF? " << (vec_val >> 8) << endl;
				wreg(HV_X86_RIP, rreg(HV_X86_RIP) + 2);
				break;
			}
			case VMX_REASON_IRQ:
				cout << "VMX_REASON_IRQ" << endl;
				break;
			case VMX_REASON_HLT:
				cout << "VMX_REASON_HLT" << endl;
				stop = 1;
				break;
			case VMX_REASON_EPT_VIOLATION:
				//cout << "VMX_REASON_EPT_VIOLATION" << endl;
				break;
			default:
				cout << "Unhandled VM exit: 0x" << hex << exit_reason << endl;
				stop = 1;
		}
	} while(!stop);
	cout << "Final EIP: " << hex << rreg(HV_X86_RIP) << endl;
}
