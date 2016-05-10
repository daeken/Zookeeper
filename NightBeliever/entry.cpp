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

	// Some XBEs appear to try to patch kernel stuff,
	// but this hack is enough to terminate that safely
	auto hack = (uint32_t *) map((void *) 0x80010000, 1);
	auto hack2 = new uint8_t[0x20];
	hack[0x3c / 4] = (uint32_t) hack2 + 0x7FFF0000;

	log("Calling entrypoint.");
	auto ep = (xbe_ep_t) xbe->oep;
	ep();

	log("Returned from entrypoint.");
}
