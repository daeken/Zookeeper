#include "Zookeeper.hpp"

Box *box;

Box::Box() {
	box = this;
	uint8_t *mem, *kmem;
	bailout(!(mem = (uint8_t *) valloc(RAM_SIZE)));
	bailout(!(kmem = (uint8_t *) valloc(KRAM_SIZE)));

	cpu = new Cpu(mem, kmem);
	hypercall = new Hypercall();

	hm = new HandleManager();
	pm = new PageManager();
	pm->add_region(0, 64 * ONE_MB);
	tm = new ThreadManager();
	io = new IOManager();

	debugger = new Debugger();

	gpu = new Gpu();
}

void Box::add_mmio(uint32_t base, uint32_t pages, Device *dev) {
	auto memblock = new uint8_t[pages * PAGE_SIZE];
	cpu->hv->map_phys(memblock, base, pages * PAGE_SIZE);
	for(auto i = 0; i < pages; ++i) {
		mmio[base] = dev;
		dev->mmioBuffers[base] = memblock;
		cpu->map_pages(base, base, 1, false); // Pages are not marked present
		base += PAGE_SIZE;
		memblock += PAGE_SIZE;
	}
}

void Box::add_port(uint32_t port, Device *dev) {
	ports[port] = dev;
}

void Box::add_pci(uint16_t bus, uint16_t slot, Device *dev) {
	auto addr = (((uint32_t) bus) << 16) | slot;
	pci[addr] = dev;
}
