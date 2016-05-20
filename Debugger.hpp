#pragma once
#include "Zookeeper.hpp"

void break_in(bool fatal=false);

class Breakpoint {
public:
	Breakpoint(uint32_t _addr);
	void enable();
	void disable();

	uint32_t addr;
	uint8_t saved;
	bool enabled;
};

class Debugger {
public:
	Debugger();
	void dump_stack();
	void stack_trace();
	void enter(uint32_t reason = -1);
	void reenable();

	map<uint32_t, Breakpoint *> breakpoints;
};
