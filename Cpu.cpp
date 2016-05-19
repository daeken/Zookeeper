#include "Zookeeper.hpp"

void gdt_encode(uint8_t *gdt, int entry, uint32_t base, uint32_t limit, uint8_t type) {
	gdt += 8 * entry;
	if(limit > 65536) {
		limit >>= 12;
		gdt[6] = 0xc0;
	} else
		gdt[6] = 0x40;

    gdt[0] = limit & 0xFF;
    gdt[1] = (limit >> 8) & 0xFF;
    gdt[6] |= (limit >> 16) & 0xF;
 
    gdt[2] = base & 0xFF;
    gdt[3] = (base >> 8) & 0xFF;
    gdt[4] = (base >> 16) & 0xFF;
    gdt[7] = (base >> 24) & 0xFF;
 
    gdt[5] = type;
}

Cpu::Cpu(uint8_t *ram, uint8_t *kram) {
	mem = ram;
	kmem = kram;

	hv = new HVImpl();
	memset(mem, 0, RAM_SIZE);
	hv->map_phys(mem, 0x00000000, RAM_SIZE);
	hv->map_phys(kmem, 0xc0000000, KRAM_SIZE);

	auto directory = 64*1024*1024; // Page directory base
	auto dir = (uint32_t *) (mem + directory);
	for(auto i = 0; i < 1024; ++i) {
		dir[i] = (directory + 4096 + 4096 * i) | 0x7;
		auto table = (uint32_t *) (mem + (dir[i] & ~0xFFF));
		for(auto j = 0; j < 1024; ++j) {
			table[j] = 0x0;
		}
	}
	hv->reg(CR3, directory);
	hv->reg(CR0, 0x80000000 | 0x20 | 0x01); // Paging | NE | PE

	auto gdt = ram + 96*1024*1024;
	memset(gdt, 0, 0x10000);
	gdt_encode(gdt, 0, 0, 0, 0); // Null entry
	gdt_encode(gdt, 1, 0, 0xffffffff, 0x9A); // Code
	gdt_encode(gdt, 2, 0, 0xffffffff, 0x92); // Data
	hv->reg(GDT_LIMIT, 0xFFFF);
	hv->reg(GDT_BASE, 96*1024*1024);
	map_pages(96 * 1024 * 1024, 96 * 1024 * 1024, 16);

	hv->reg(CR4, 0x2000);
}

void Cpu::map_pages(uint32_t virt, uint32_t phys, uint32_t count) {
	auto dir = (uint32_t *) (mem + 64*1024*1024);
	for(auto i = 0; i < count; ++i) {
		auto table = (uint32_t *) (mem + (dir[virt >> 22] & ~0xFFF));
		table[(virt >> 12) & 0x3ff] = phys | 0x7;
		virt += 4096;
		phys += 4096;
	}
	hv->invalidate_tlb(); // Do we really need to do this all the time?
}

uint32_t Cpu::virt2phys(uint32_t addr) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0)
		return addr;

	auto directory = (uint32_t *) (mem + cr3);
	auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
	return (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
}

bool Cpu::is_mapped(uint32_t addr) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0)
		return true;

	auto directory = (uint32_t *) (mem + cr3);
	auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
	return (table[(addr >> 12) & 0x3ff] & 1) == 1;
}

void Cpu::read_memory(uint32_t addr, uint32_t size, void *buffer) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0) {
		if(addr >= 0xc0000000)
			memcpy(buffer, &kmem[addr - 0xc0000000], size);
		else
			memcpy(buffer, &mem[addr], size);
		return;
	}
	auto buf = (uint8_t *) buffer;
	auto directory = (uint32_t *) (mem + cr3);
	for(auto i = 0; i < size; ++i) {
		auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
		auto paddr = (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
		if(paddr >= 0xc0000000)
			*(buf++) = kmem[paddr - 0xc0000000];
		else
			*(buf++) = mem[paddr];
		++addr;
	}
}

void Cpu::write_memory(uint32_t addr, uint32_t size, void *buffer) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0) {
		if(addr >= 0xc0000000)
			memcpy(&kmem[addr - 0xc0000000], buffer, size);
		else
			memcpy(&mem[addr], buffer, size);
		return;
	}
	auto buf = (uint8_t *) buffer;
	auto directory = (uint32_t *) (mem + cr3);
	for(auto i = 0; i < size; ++i) {
		auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~0xFFF));
		auto paddr = (table[(addr >> 12) & 0x3ff] & ~0xFFF) + (addr & 0xFFF);
		if(paddr >= 0xc0000000)
			kmem[paddr - 0xc0000000] = *(buf++);
		else
			mem[paddr] = *(buf++);
		++addr;
	}
}

Cpu::~Cpu() {
	delete hv;
}

#define TASK_TIMER 20 // Milliseconds
#define TASK_INTERRUPT 80

uint64_t systime() {
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void Cpu::run(uint32_t eip) {
	hv->reg(EIP, eip);
	hv->reg(EFLAGS, 0x2);

	box->debugger->enter(0);

	auto last_time = systime();

	auto swap = true;
	do {
		if(break_in) {
			box->debugger->enter();
			break_in = false;
		}
		auto exit = hv->enter();

		switch (exit.reason) {
			case Interrupt: {
				if(exit.interrupt_no == 3) {
					box->debugger->enter();
				} else {
					cout << "Unknown interrupt. " << dec << exit.interrupt_no << endl;
					box->debugger->enter();
					stop = true;
				}
				break;
			}
			case Exception: {
				switch(exit.interrupt_no) {
					case 1: { // Single step
						auto flags = hv->reg(EFLAGS);
						hv->reg(EFLAGS, flags & ~(1 << 8));
						if(single_step == 2) { // Requested
							single_step = 0;
							box->debugger->enter();
						}
						else
							box->debugger->reenable();
						swap = true;
						break;
					}
					case 6: {
						cout << "Invalid opcode" << endl;
						stop = true;
						break;
					}
					case 14: {
						cout << format("Page fault reading %08x") % exit.address << endl;
						stop = true;
						break;
					}
					default:
						cout << "Unknown exception. " << dec << exit.interrupt_no << endl;
						stop = true;
				}
				break;
			}

			case VmCall: {
				hv->reg(EIP, hv->reg(EIP) + 3);
				hypercall_dispatch(hv->reg(EAX), hv->reg(EDX));
				break;
			}
			case TripleFault:
				cout << "Triple fault!" << endl;
				stop = true;
				break;
			case Hlt:
				cout << "HLT" << endl;
				stop = true;
				break;
			case Ignore:
				break;
			default:
				stop = true;
		}

		if(single_step) {
			if(single_step == 1)
				single_step = 0;
			auto flags = hv->reg(EFLAGS);
			hv->reg(EFLAGS, flags | (1 << 8));
			swap = false;
		}

		if(swap) {
			auto cur_time = systime();
			if(cur_time >= last_time + TASK_TIMER) {
				box->tm->next();
				last_time = cur_time;
			}
		}
	} while(!stop);
	box->debugger->enter();
}
