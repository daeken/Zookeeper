#include "Zookeeper.hpp"

ThreadManager::ThreadManager() {
	auto thread = make_shared<Thread>();
	thread->id = 0;
	threads.push_back(thread);
	iterator = threads.begin();
}

uint32_t ThreadManager::create(uint32_t eip, uint32_t esp) {
	cout << "Creating thread at " << hex << eip << " stack at " << esp << endl;
	auto thread = make_shared<Thread>();
	(*iterator)->save();
	thread->eflags = (*iterator)->eflags; // XXX: Copying current eflags state.  Add default
	thread->id = ++tid;
	thread->eip = eip;
	thread->esp = esp;
	threads.push_back(thread);
	return thread->id;
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
	REG(CS, cs); \
	REG(SS, ss); \
	REG(DS, ds); \
	REG(ES, es); \
	REG(FS, fs); \
	REG(GS, gs); \
} while(0)

void Thread::save() {
#define REG(reg, name) (name = (uint32_t) box->cpu->rreg(HV_X86_##reg))
	REGMAGIC();
#undef REG
#ifdef THREADDEBUG
	cout << "Saved thread" << endl;
#define REG(reg, name) cout << "REG " << #name << " == 0x" << hex << name << endl;
	REGMAGIC();
#undef REG
#endif
}

void Thread::restore() {
#define REG(reg, name) (box->cpu->wreg(HV_X86_##reg, (uint64_t) name))
	REGMAGIC();
#undef REG
#ifdef THREADDEBUG
	cout << "Restored thread" << endl;
#define REG(reg, name) cout << "REG " << #name << " == 0x" << hex << name << endl;
	REGMAGIC();
#undef REG
#endif
}
