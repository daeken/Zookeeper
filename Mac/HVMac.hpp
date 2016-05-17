#include "Zookeeper.hpp"

class HVMac : public HV {
public:
	HVMac();
	~HVMac();
	void map_phys(void *memory, uint32_t base, uint32_t size);
	void invalidate_tlb();
	uint32_t reg(HVReg reg);
	void reg(HVReg reg, uint32_t val);
	exit_t enter();
private:
	uint32_t rreg(hv_x86_reg_t reg) {
		uint64_t v;
		bailout(hv_vcpu_read_register(vcpu, reg, &v));
		return (uint32_t) v;
	}
	void wreg(hv_x86_reg_t reg, uint32_t v) {
		bailout(hv_vcpu_write_register(vcpu, reg, (uint32_t) v));
	}

	uint64_t rvmcs(uint32_t field) {
		uint64_t v;
		bailout(hv_vmx_vcpu_read_vmcs(vcpu, field, &v));
		return v;
	}
	void wvmcs(uint32_t field, uint64_t v) {
		bailout(hv_vmx_vcpu_write_vmcs(vcpu, field, v));
	}

	hv_vcpuid_t vcpu;
};

#define HVImpl HVMac
