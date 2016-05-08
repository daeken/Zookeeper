#include "NightBeliever.hpp"

typedef void(*xbe_ep_t)();

void entrypoint() {
	log("NightBeliever initializing...");
	init_idt();

	log("Idle.");

	auto ep = (xbe_ep_t) get_entrypoint();
	ep();

	log("Returned from entrypoint.");
}
