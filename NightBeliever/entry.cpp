#include "NightBeliever.hpp"

typedef void(*xbe_ep_t)();

void entrypoint() {
	log("NightBeliever initializing...");

	auto xbe = get_xbebase();
	auto thunk = (uint32_t *) xbe->thunk;
	while(*thunk) {
		*thunk = thunk_lookup(*thunk);
		++thunk;
	}
	global_tls = (XbeTLS_t *) xbe->tls;

	init_tib(0);

	log("Calling entrypoint.");
	auto ep = (xbe_ep_t) xbe->oep;
	ep();

	log("Returned from entrypoint.");
}
