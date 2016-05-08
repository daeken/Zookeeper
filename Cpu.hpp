#pragma once

#include "Zookeeper.hpp"

#define cap2ctrl(cap, ctrl) ((ctrl) | ((cap) & 0xffffffff)) & ((cap) >> 32)
#define VMCS_PRI_PROC_BASED_CTLS_HLT           (1 << 7)
#define VMCS_PRI_PROC_BASED_CTLS_CR8_LOAD      (1 << 19)
#define VMCS_PRI_PROC_BASED_CTLS_CR8_STORE     (1 << 20)

class Cpu {
public:
	Cpu(uint8_t *ram, uint8_t *kram);
	~Cpu();
	void run(uint32_t eip);
	void map_pages(uint32_t virt, uint32_t phys, uint32_t count);
	
	uint32_t virt2phys(uint32_t addr);
	void read_memory(uint32_t addr, uint32_t size, void *buffer);
	template<typename T> T read_memory(uint32_t addr) {
		T value;
		read_memory(addr, sizeof(T), &value);
		return value;
	}

	void write_memory(uint32_t addr, uint32_t size, void *buffer);
	template<typename T> void write_memory(uint32_t addr, T &value) {
		write_memory(addr, sizeof(T), &value);
	}

	uint64_t rdmsr(uint32_t msr);
	void wrmsr(uint32_t msr, uint64_t val);

	/* read GPR */
	uint64_t rreg(hv_x86_reg_t reg) {
		uint64_t v;
		bailout(hv_vcpu_read_register(vcpu, reg, &v));
		return v;
	}
	/* write GPR */
	void wreg(hv_x86_reg_t reg, uint64_t v) {
		bailout(hv_vcpu_write_register(vcpu, reg, v));
	}
	/* read VMCS field */
	uint64_t rvmcs(uint32_t field) {
		uint64_t v;
		bailout(hv_vmx_vcpu_read_vmcs(vcpu, field, &v));
		return v;
	}
	/* write VMCS field */
	void wvmcs(uint32_t field, uint64_t v) {
		bailout(hv_vmx_vcpu_write_vmcs(vcpu, field, v));
	}

	hv_vcpuid_t vcpu;
	uint8_t *mem, *kmem;
};
