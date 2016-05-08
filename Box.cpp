#include "Zookeeper.hpp"

Box *box;

Box::Box() {
	uint8_t *mem, *kmem;
	bailout(!(mem = (uint8_t *) valloc(RAM_SIZE)));
	bailout(!(kmem = (uint8_t *) valloc(KRAM_SIZE)));

	cpu = new Cpu(mem, kmem);

	pm = new PageManager();
	pm->add_region(1024*1024, RAM_SIZE - 1024*1024);
}
