#include "Zookeeper.hpp"

ThreadManager::ThreadManager() {
	threads.push_back(make_shared<Thread>());
	iterator = threads.begin();
}

uint32_t ThreadManager::create(uint32_t eip, uint32_t esp) {
	cout << "Creating thread at " << hex << eip << " stack at " << esp << endl;
	auto thread = make_shared<Thread>();
	(*iterator)->save();
	thread->eflags = (*iterator)->eflags; // Copy current eflags state
	thread->eip = eip;
	thread->esp = esp;
	threads.push_back(thread);
	return 0; // XXX: Return a tid
}

void ThreadManager::next() {
	if(threads.size() < 2)
		return;

	(*iterator)->save();
	iterator++;
	if(iterator == threads.end())
		iterator = threads.begin();
	(*iterator)->restore();
}

#define REGMAGIC() do {\
	REG(RIP, eip); \
	REG(RFLAGS, eflags); \
	REG(RAX, eax); \
	REG(RCX, ecx); \
	REG(RDX, edx); \
	REG(RBX, ebx); \
	REG(RSI, esi); \
	REG(RDI, edi); \
	REG(RSP, esp); \
	REG(RBP, ebp); \
} while(0)

/*
	REG(CS, cs); \
	REG(SS, ss); \
	REG(DS, ds); \
	REG(ES, es); \
	REG(FS, fs); \
	REG(GS, gs);
*/

void Thread::save() {
#define REG(reg, name) (name = (uint32_t) box->cpu->rreg(HV_X86_##reg))
	REGMAGIC();
#undef REG
/*	cout << "Saved thread at 0x" << hex << eip << endl;
#define REG(reg, name) cout << "REG " << #name << " == 0x" << hex << name << endl;
	REGMAGIC();
#undef REG*/
}

void Thread::restore() {
#define REG(reg, name) (box->cpu->wreg(HV_X86_##reg, (uint64_t) name))
	REGMAGIC();
#undef REG
/*	cout << "Restored thread at 0x" << hex << eip << endl;
#define REG(reg, name) cout << "REG " << #name << " == 0x" << hex << name << endl;
	REGMAGIC();
#undef REG*/
}
