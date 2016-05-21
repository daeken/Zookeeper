#include "Zookeeper.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
using namespace boost::algorithm;

void break_in(bool fatal) {
	if(fatal)
		box->cpu->stop = true;
	else
		box->cpu->do_break_in = true;
}

Breakpoint::Breakpoint(uint32_t _addr) : addr(_addr) {
	enable();
}

void Breakpoint::enable() {
	if(enabled)
		return;
	saved = box->cpu->read_memory<uint8_t>(addr);
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

	auto ebp = box->cpu->hv->reg(EBP);
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
	auto esp = box->cpu->hv->reg(ESP);
	for(auto i = 0; i < MAX_LENGTH; ++i)
		cout << format("%08x") % box->cpu->read_memory<uint32_t>(esp + i * 4) << endl;
}

void Debugger::enter(uint32_t reason) {
	auto eip = box->cpu->hv->reg(EIP);

	if(IN(eip, breakpoints)) {
		cout << format("Hit breakpoint at 0x%08x") % eip << endl;
		breakpoints[eip]->disable();
		box->cpu->single_step = 1;
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
		else if(cmd == "s" || cmd == "step") {
			box->cpu->single_step = 2;
			break;
		} else if(cmd == "b" || cmd == "bp" || cmd == "break") {
			assert(v.size() >= 2);
			auto addr = stoul(v[1], nullptr, 16);
			if(IN(addr, breakpoints))
				cout << format("Breakpoint already exists at %08x") % addr << endl;
			else {
				breakpoints[addr] = new Breakpoint(addr);
				cout << format("Added breakpoint at addr %08x") % addr << endl;
			}
		} else if(cmd == "trace") {
			stack_trace();
		} else if(cmd == "stack") {
			dump_stack();
		} else if(cmd == "r" || cmd == "regs") {
			cout << format("eax  %08x    ebx  %08x") % box->cpu->hv->reg(EAX) % box->cpu->hv->reg(EBX) << endl;
			cout << format("ecx  %08x    edx  %08x") % box->cpu->hv->reg(ECX) % box->cpu->hv->reg(EDX) << endl;
			cout << format("edi  %08x    esi  %08x") % box->cpu->hv->reg(EDI) % box->cpu->hv->reg(ESI) << endl;
			cout << format("ebp  %08x    esp  %08x") % box->cpu->hv->reg(EBP) % box->cpu->hv->reg(ESP) << endl;
			cout << format("eflags  %08x") % box->cpu->hv->reg(EFLAGS) << endl;
		} else if(cmd == "rm" || cmd == "read") {
			string type = "u32";
			auto offset = 1;
			if(v.size() == 3) {
				type = v[1];
				offset = 2;
			}
			auto addr = stoul(v[offset], nullptr, 16);
			if(type == "u8")
				cout << format("*0x%08x == 0x%02x") % addr % (uint32_t) box->cpu->read_memory<uint8_t>(addr) << endl;
			else if(type == "u16")
				cout << format("*0x%08x == 0x%04x") % addr % (uint32_t) box->cpu->read_memory<uint16_t>(addr) << endl;
			else if(type == "u32")
				cout << format("*0x%08x == 0x%08x") % addr % box->cpu->read_memory<uint32_t>(addr) << endl;
			else
				cout << "Unknown type (try u8, u16, or u32)" << endl;
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
