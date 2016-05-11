#include "Zookeeper.hpp"

Box *box;

Box::Box() {
	uint8_t *mem, *kmem;
	bailout(!(mem = (uint8_t *) valloc(RAM_SIZE)));
	bailout(!(kmem = (uint8_t *) valloc(KRAM_SIZE)));

	cpu = new Cpu(mem, kmem);
	
	hypercall = new Hypercall();

	pm = new PageManager();
	pm->add_region(0, 64 * 1024 * 1024);

	tm = new ThreadManager();

	debugger = new Debugger();
}
