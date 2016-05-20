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
