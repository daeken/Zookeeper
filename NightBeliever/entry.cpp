#include "NightBeliever.hpp"

typedef void(*xbe_ep_t)();

void entrypoint() {
	log("NightBeliever initializing...");
	init_tib();
	log("Idle.");

	auto xbe = get_xbebase();
	auto thunk = (uint32_t *) xbe->thunk;
	while(*thunk) {
		*thunk = thunk_lookup(*thunk);
		++thunk;
	}
	auto ep = (xbe_ep_t) xbe->oep;
	ep();

	log("Returned from entrypoint.");
}
