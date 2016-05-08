#include "NightBeliever.hpp"

void entrypoint() {
	log("NightBeliever initializing...");
	init_idt();

	log("Idle.");

	log("%08x", (uint32_t) map((void *) 0xdeadbeef, (void *) 0, 10));

	while(true) {
	}
}
