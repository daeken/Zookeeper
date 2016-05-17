#include "Zookeeper.hpp"

ThreadManager::ThreadManager() {
	auto thread = make_shared<Thread>();
	thread->id = 0;
	threads.push_back(thread);
	iterator = threads.begin();
}

uint32_t ThreadManager::create(uint32_t eip, uint32_t esp) {
	auto thread = make_shared<Thread>();
	box->hm->add(thread);
	thread->eflags = 2;
	thread->eip = eip;
	thread->esp = esp;
	threads.push_back(thread);
	return thread->handle;
}

void ThreadManager::terminate(uint32_t thread) {
	if(thread == -1)
		thread = current_thread();

	if(thread == current_thread())
		next();
	
	if(threads.size() == 1) {
		cout << "Last thread ending" << endl;
		box->cpu->stop = true;
		return;
	}
	
	for(auto iter = threads.begin(); iter != threads.end(); ++iter) {
		if((*iter)->handle == thread) {
			threads.erase(iter);
			(*iter)->close();
			return;
		}
	}

	cout << "Could not find thread with id " << dec << thread << endl;
	bailout(true);
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

uint32_t ThreadManager::current_thread() {
	return (*iterator)->handle;
}

#define REGMAGIC() do {\
	REG(EIP, eip); \
	REG(EFLAGS, eflags); \
	REG(EAX, eax); \
	REG(ECX, ecx); \
	REG(EDX, edx); \
	REG(EBX, ebx); \
	REG(ESI, esi); \
	REG(EDI, edi); \
	REG(ESP, esp); \
	REG(EBP, ebp); \
	REG(CS, cs); \
	REG(SS, ss); \
	REG(DS, ds); \
	REG(ES, es); \
	REG(FS, fs); \
	REG(GS, gs); \
} while(0)

void Thread::save() {
#define REG(_reg, name) (name = box->cpu->hv->reg(_reg))
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
#define REG(_reg, name) (box->cpu->hv->reg(_reg, name))
	REGMAGIC();
#undef REG
#ifdef THREADDEBUG
	cout << "Restored thread" << endl;
#define REG(reg, name) cout << "REG " << #name << " == 0x" << hex << name << endl;
	REGMAGIC();
#undef REG
#endif
}
