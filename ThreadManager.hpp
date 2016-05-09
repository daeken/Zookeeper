#pragma once
#include "Zookeeper.hpp"

class Thread {
public:
	void save();
	void restore();

	uint32_t id;

	uint32_t eip, eflags, eax, ecx, edx, ebx;
	uint32_t esi, edi, esp, ebp;

	uint32_t cs, ss, ds, es, fs, gs;
};

class ThreadManager {
public:
	ThreadManager();
	uint32_t create(uint32_t eip, uint32_t esp);
	void next();

	list<shared_ptr<Thread>> threads;
	list<shared_ptr<Thread>>::iterator iterator;
	uint32_t tid = 0;
};
