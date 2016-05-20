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
	hv->map_phys(kmem, KBASE, KRAM_SIZE);

	auto directory = 64*ONE_MB; // Page directory base
	auto dir = (uint32_t *) (mem + directory);
	for(auto i = 0; i < 1024; ++i) {
		dir[i] = (directory + PAGE_SIZE + PAGE_SIZE * i) | 0x7;
		auto table = (uint32_t *) (mem + (dir[i] & ~PAGE_MASK));
		for(auto j = 0; j < 1024; ++j) {
			table[j] = 0x0;
		}
	}
	hv->reg(CR3, directory);
	hv->reg(CR0, 0x80000000 | 0x20 | 0x01); // Paging | NE | PE

	auto gdt = ram + 96*ONE_MB;
	memset(gdt, 0, 0x10000);
	gdt_encode(gdt, 0, 0, 0, 0); // Null entry
	gdt_encode(gdt, 1, 0, 0xffffffff, 0x9A); // Code
	gdt_encode(gdt, 2, 0, 0xffffffff, 0x92); // Data
	hv->reg(GDT_LIMIT, 0xFFFF);
	hv->reg(GDT_BASE, 96*ONE_MB);
	map_pages(96 * ONE_MB, 96 * ONE_MB, 16);

	hv->reg(CR4, 0x2000);
}

void Cpu::map_pages(uint32_t virt, uint32_t phys, uint32_t count, bool present) {
	auto dir = (uint32_t *) (mem + 64*ONE_MB);
	for(auto i = 0; i < count; ++i) {
		auto table = (uint32_t *) (mem + (dir[virt >> 22] & ~PAGE_MASK));
		table[(virt >> 12) & 0x3ff] = phys | 0x6 | (present ? 1 : 0);
		virt += PAGE_SIZE;
		phys += PAGE_SIZE;
	}
	hv->invalidate_tlb(); // Do we really need to do this all the time?
}

void Cpu::map_io(uint32_t base, uint32_t pages, MMIOReceiver *recv) {
	auto memblock = new uint8_t[pages * PAGE_SIZE];
	hv->map_phys(memblock, base, pages * PAGE_SIZE);
	for(auto i = 0; i < pages; ++i) {
		mmio[base] = recv;
		recv->buffers[base] = memblock;
		map_pages(base, base, 1, false); // Pages are not marked present
		base += PAGE_SIZE;
		memblock += PAGE_SIZE;
	}
}

void Cpu::flip_page(uint32_t base, bool val) {
	auto dir = (uint32_t *) (mem + 64*ONE_MB);
	auto table = (uint32_t *) (mem + (dir[base >> 22] & ~PAGE_MASK));
	table[(base >> 12) & 0x3ff] = (table[(base >> 12) & 0x3ff] & ~1) | (val ? 1 : 0);
	hv->invalidate_tlb();
}

uint32_t Cpu::virt2phys(uint32_t addr) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0)
		return addr;

	auto directory = (uint32_t *) (mem + cr3);
	auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~PAGE_MASK));
	return (table[(addr >> 12) & 0x3ff] & ~PAGE_MASK) + (addr & PAGE_MASK);
}

bool Cpu::is_mapped(uint32_t addr) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0)
		return true;

	auto directory = (uint32_t *) (mem + cr3);
	auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~PAGE_MASK));
	return (table[(addr >> 12) & 0x3ff] & 1) == 1;
}

void Cpu::read_memory(uint32_t addr, uint32_t size, void *buffer) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0) {
		if(addr >= KBASE)
			memcpy(buffer, &kmem[addr - KBASE], size);
		else
			memcpy(buffer, &mem[addr], size);
		return;
	}
	auto buf = (uint8_t *) buffer;
	auto directory = (uint32_t *) (mem + cr3);
	for(auto i = 0; i < size; ++i) {
		auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~PAGE_MASK));
		auto paddr = (table[(addr >> 12) & 0x3ff] & ~PAGE_MASK) + (addr & PAGE_MASK);
		if(paddr >= KBASE)
			*(buf++) = kmem[paddr - KBASE];
		else
			*(buf++) = mem[paddr];
		++addr;
	}
}

void Cpu::write_memory(uint32_t addr, uint32_t size, void *buffer) {
	auto cr3 = hv->reg(CR3);
	if(cr3 == 0) {
		if(addr >= KBASE)
			memcpy(&kmem[addr - KBASE], buffer, size);
		else
			memcpy(&mem[addr], buffer, size);
		return;
	}
	auto buf = (uint8_t *) buffer;
	auto directory = (uint32_t *) (mem + cr3);
	for(auto i = 0; i < size; ++i) {
		auto table = (uint32_t *) (mem + (directory[addr >> 22] & ~PAGE_MASK));
		auto paddr = (table[(addr >> 12) & 0x3ff] & ~PAGE_MASK) + (addr & PAGE_MASK);
		if(paddr >= KBASE)
			kmem[paddr - KBASE] = *(buf++);
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
	uint32_t in_mmio;
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
						switch(single_step) {
							case 1: { // Debugger requested
								box->debugger->reenable();
								break;
							}
							case 2: { // User requested
								single_step = 0;
								box->debugger->enter();
								break;
							}
							case 3: { // MMIO Read
								auto page = in_mmio & ~PAGE_MASK;
								flip_page(page, false);
								single_step = 0;
								in_mmio = 0;
								break;
							}
							case 4: { // MMIO Write
								auto page = in_mmio & ~PAGE_MASK;
								flip_page(page, false);
								auto dev = mmio[page];
								auto buf = dev->buffers[page];
								auto off = in_mmio & PAGE_MASK;
								volatile auto val = (uint32_t *) ((uint8_t *) buf + off);
								dev->write(in_mmio, *val);
								single_step = 0;
								in_mmio = 0;
								break;
							}
						}
						swap = true;
						break;
					}
					case 6: {
						cout << "Invalid opcode" << endl;
						stop = true;
						break;
					}
					case 14: {
						auto page = exit.address & ~PAGE_MASK;
						if(IN(page, mmio)) {
							auto off = exit.address & PAGE_MASK;
							auto dev = mmio[page];
							auto buf = dev->buffers[page];
							auto write = FLAG(exit.error_code, 2);
							in_mmio = exit.address;
							flip_page(page, true);
							if(write) {
								single_step = 4;
							} else {
								volatile auto val = (uint32_t *) ((uint8_t *) buf + off);
								*val = dev->read(exit.address);
								single_step = 3;
							}
						} else {
							cout << format("Page fault reading %08x") % exit.address << endl;
							stop = true;
						}
						break;
					}
					default:
						cout << "Unknown exception. " << dec << exit.interrupt_no << endl;
						stop = true;
				}
				break;
			}

			case VmCall: {
				hv->reg(EIP, hv->reg(EIP) + exit.instruction_length);
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
			case PortIO: {
				hv->reg(EIP, hv->reg(EIP) + exit.instruction_length);
				cout << "Port IO: " << hex << exit.port << endl;
				break;
			}
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
