#pragma once
#include "NightBeliever.hpp"

extern XbeTLS_t *global_tls;

void init_tib(uint32_t tid);

inline KPCR *get_tib() {
	KPCR *tib;
	__asm("mov %%fs:(0x1c), %0" : "=r"(tib));
	return tib;
}

inline uint32_t get_thread_id() {
	return ((ETHREAD *) get_tib()->PrcbData.CurrentThread)->UniqueThread;
}
