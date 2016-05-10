#include "Zookeeper.hpp"

Breakpoint::Breakpoint(uint32_t _addr) : addr(_addr) {
	saved = box->cpu->read_memory<uint8_t>(addr);
	enable();
}

void Breakpoint::enable() {
	if(enabled)
		return;
	box->cpu->write_memory<uint8_t>(addr, 0xCC);
	box->cpu->invalidate_tlb();
	enabled = true;
}

void Breakpoint::disable() {
	if(!enabled)
		return;
	box->cpu->write_memory(addr, saved);
	box->cpu->invalidate_tlb();
	enabled = false;
}

Debugger::Debugger() {
}

#define MAX_FRAMES 8
void Debugger::stack_trace() {
	cout << "Stack trace:" << endl;

	auto ebp = box->cpu->rreg(HV_X86_RBP);
	for(auto i = 0; i < MAX_FRAMES; ++i) {
		if(ebp == 0)
			break;
		auto eip = box->cpu->read_memory<uint32_t>(ebp + 4);
		cout << hex << eip << endl;
		ebp = box->cpu->read_memory<uint32_t>(ebp);
	}

	cout << endl;
}

#define MAX_LENGTH 20
void Debugger::dump_stack() {
	auto esp = box->cpu->rreg(HV_X86_RSP);
	for(auto i = 0; i < MAX_LENGTH; ++i)
		cout << format("%08x") % box->cpu->read_memory<uint32_t>(esp + i * 4) << endl;
}

void Debugger::enter(uint32_t reason) {
	auto eip = box->cpu->rreg(HV_X86_RIP);

	if(breakpoints.find(eip) != breakpoints.end()) {
		cout << format("Hit breakpoint at 0x%08x") % eip << endl;
		breakpoints[eip]->disable();
		box->cpu->single_step = true;
	}

	while(1) {
		cout << format("[%i] %08x > ") % box->tm->current_thread() % eip << flush;

		string line;
		if(!getline(cin, line)) {
			cout << endl;
			exit(0);
		}
		line = boost::regex_replace(line, boost::regex("[\\s]{2,}"), " ");

		vector<string> v;
		split(v, line, is_space());
		if(v.size() == 0 || v[0] == "")
			continue;
		
		auto cmd = v[0];
		if(cmd == "c" || cmd == "cont" || cmd == "continue")
			break;
		else if(cmd == "b" || cmd == "bp" || cmd == "break") {
			assert(v.size() >= 2);
			auto addr = stoul(v[1], nullptr, 16);
			if(breakpoints.find(addr) != breakpoints.end())
				cout << format("Breakpoint already exists at %08x") % addr << endl;
			else {
				breakpoints[addr] = new Breakpoint(addr);
				cout << format("Added breakpoint at addr %08x") % addr << endl;
			}
		} else if(cmd == "t" || cmd == "trace") {
			stack_trace();
		} else if(cmd == "s" || cmd == "stack") {
			dump_stack();
		} else if(cmd == "r" || cmd == "regs") {
			cout << format("eax  %08x    ebx  %08x") % box->cpu->rreg(HV_X86_RAX) % box->cpu->rreg(HV_X86_RBX) << endl;
			cout << format("ecx  %08x    edx  %08x") % box->cpu->rreg(HV_X86_RCX) % box->cpu->rreg(HV_X86_RDX) << endl;
			cout << format("edi  %08x    esi  %08x") % box->cpu->rreg(HV_X86_RDI) % box->cpu->rreg(HV_X86_RSI) << endl;
			cout << format("ebp  %08x    esp  %08x") % box->cpu->rreg(HV_X86_RBP) % box->cpu->rreg(HV_X86_RSP) << endl;
			cout << format("eflags  %08x") % box->cpu->rreg(HV_X86_RFLAGS) << endl;
		} else {
			cout << "Unknown command" << endl;
		}
	}
}

void Debugger::reenable() {
	for(auto bp : breakpoints) {
		bp.second->enable();
	}
}
