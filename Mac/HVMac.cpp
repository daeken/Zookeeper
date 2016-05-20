#include "Zookeeper.hpp"

HV::~HV() {} // This has to be defined, but there's no use in making a new cpp file.

#define cap2ctrl(cap, ctrl) ((ctrl) | ((cap) & 0xffffffff)) & ((cap) >> 32)
#define VMCS_PRI_PROC_BASED_CTLS_HLT           (1 << 7)
#define VMCS_PRI_PROC_BASED_CTLS_CR8_LOAD      (1 << 19)
#define VMCS_PRI_PROC_BASED_CTLS_CR8_STORE     (1 << 20)

HVMac::HVMac() {
	bailout(hv_vm_create(HV_VM_DEFAULT));
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
}

HVMac::~HVMac() {
	bailout(hv_vcpu_destroy(vcpu));
	bailout(hv_vm_unmap(0, RAM_SIZE));
	bailout(hv_vm_destroy());
}

void HVMac::map_phys(void *memory, uint32_t base, uint32_t size) {
	bailout(hv_vm_map(memory, base, size, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));
}

void HVMac::invalidate_tlb() {
	hv_vcpu_invalidate_tlb(vcpu);
}

#define REG_MAGIC() \
REG(EIP, RIP) \
REG(EFLAGS, RFLAGS) \
REG(EAX, RAX) \
REG(ECX, RCX) \
REG(EDX, RDX) \
REG(EBX, RBX) \
REG(ESI, RSI) \
REG(EDI, RDI) \
REG(ESP, RSP) \
REG(EBP, RBP) \
REG1(CS) \
REG1(SS) \
REG1(DS) \
REG1(ES) \
REG1(FS) \
REG1(GS) \
REG1(IDT_BASE) \
REG1(IDT_LIMIT) \
REG1(GDT_BASE) \
REG1(GDT_LIMIT) \
REG1(LDTR) \
REG1(LDT_BASE) \
REG1(LDT_LIMIT) \
REG1(LDT_AR) \
REG1(TR) \
REG1(TSS_BASE) \
REG1(TSS_LIMIT) \
REG1(TSS_AR) \
REG1(CR0) \
REG1(CR1) \
REG1(CR2) \
REG1(CR3) \
REG1(CR4) \
REG1(DR0) \
REG1(DR1) \
REG1(DR2) \
REG1(DR3) \
REG1(DR4) \
REG1(DR5) \
REG1(DR6) \
REG1(DR7) \
REG1(TPR) \
REG1(XCR0)

uint32_t HVMac::reg(HVReg reg) {
#define REG(a, b) case a: return rreg(HV_X86_##b);
#define REG1(a) case a: return rreg(HV_X86_##a);
	switch(reg) {
		REG_MAGIC()
	}
#undef REG
#undef REG1
}
void HVMac::reg(HVReg reg, uint32_t val) {
#define REG(a, b) case a: wreg(HV_X86_##b, val); break;
#define REG1(a) case a: wreg(HV_X86_##a, val); break;
	switch(reg) {
		REG_MAGIC()
	}
#undef REG
#undef REG1
}

exit_t HVMac::enter() {
	exit_t _exit;

	bailout(hv_vcpu_run(vcpu));
	auto exit_reason = rvmcs(VMCS_RO_EXIT_REASON);
	if(exit_reason & 0x80000000) {
		_exit.reason = EntryFailed;
		return _exit;
	}

	_exit.instruction_length = rvmcs(VMCS_RO_VMEXIT_INSTR_LEN);
	auto qual = _exit.address = rvmcs(VMCS_RO_EXIT_QUALIFIC);

	switch(exit_reason) {
		case VMX_REASON_EXC_NMI: {
			auto vec_val = rvmcs(VMCS_RO_VMEXIT_IRQ_INFO) & 0xFFFF;
			_exit.error_code = rvmcs(VMCS_RO_VMEXIT_IRQ_ERROR);
			switch((vec_val >> 8) & 7) {
				case 6: {
					_exit.reason = Interrupt;
					_exit.interrupt_no = vec_val & 0xFF;
					break;
				}
				case 3: {
					_exit.reason = Exception;
					_exit.interrupt_no = vec_val & 0xFF;
					break;
				}
				default: {
					cout << "Unknown NMI class. " << hex << vec_val << endl;
					_exit.reason = Ignore;
				}
			}
			break;
		}
		case VMX_REASON_VMCALL: {
			_exit.reason = VmCall;
			break;
		}
		case VMX_REASON_TRIPLE_FAULT: {
			_exit.reason = TripleFault;
			break;
		}
		case VMX_REASON_HLT: {
			_exit.reason = Hlt;
			break;
		}
		case VMX_REASON_IO: {
			_exit.reason = PortIO;
			_exit.access_size = ((qual & 3) + 1) << 3;
			_exit.port = qual >> 16;
			_exit.port_direction = ((qual >> 3) & 1) == 0;
			break;
		}
		case VMX_REASON_IRQ:
		case VMX_REASON_EPT_VIOLATION: {
			_exit.reason = Ignore;
			break;
		}
		default: {
			cout << "Unknown VM exit " << dec << exit_reason << endl;
			_exit.reason = Unknown;
		}
	}
	return _exit;
}
