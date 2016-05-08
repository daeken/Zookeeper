#include "Zookeeper.hpp"

uint32_t load_multiboot(Cpu &cpu, uint32_t *header) {
	assert(header[1] & 0x10000);

	uint8_t *rel = (uint8_t *) header;

	uint32_t memsize = header[6] - header[3];
	if(memsize & 0xFFF)
		memsize = (memsize & ~0xFFF) + 0x1000;
	assert(memsize <= KRAM_SIZE);

	memcpy(cpu.kmem + (header[4] - 0xC0000000), rel + (header[4] - header[3]), memsize);
	cpu.map_pages(0xC0000000, 0xC0000000, memsize / 4096);

	return header[7];
}

uint32_t load_kernel(Cpu &cpu) {
	FILE *fp = fopen("nightbeliever.krnl", "r");
	fseek(fp, 0, SEEK_END);
	uint32_t size = ftell(fp);
	uint8_t *buf = new uint8_t[size];
	fseek(fp, 0, SEEK_SET);
	fread(buf, 1, size, fp);

	uint32_t *seek = (uint32_t *) buf;
	uint32_t *end = (uint32_t *) (buf + size);
	uint32_t entry;
	do {
		if(*seek == 0x1BADB002 && seek[2] == -(seek[0] + seek[1])) {
			if((entry = load_multiboot(cpu, seek)) != -1)
				break;
		}
	} while(++seek != end);

	if(seek == end) {
		cout << "Reached EOF before finding Multiboot header!" << endl;
	}

	delete[] buf;
	return entry;
}

int main(int argc, char **argv) {
	uint8_t *mem, *kmem;
	bailout(!(mem = (uint8_t *) valloc(RAM_SIZE)));
	bailout(!(kmem = (uint8_t *) valloc(KRAM_SIZE)));

	Cpu cpu(mem, kmem);

	uint32_t entry = load_kernel(cpu);
	
	/*cpu.map_pages(xbe.header.base, 0x80000, size / 4096);
	cpu.alloc_stack(0x10000000, 0x80000 - xbe.header.stack_commit, xbe.header.stack_commit);
	uint32_t *thunk = (uint32_t *) (mem + 0x80000 + xbe.header.thunk - xbe.header.base);
	uint32_t thunk_count = 0;
	while(*thunk != 0) {
		thunk_count++;
		thunk++;
	}
	uint8_t *thunk_out = mem;
	thunk = (uint32_t *) (mem + 0x80000 + xbe.header.thunk - xbe.header.base);
	for(int i = 0; i < thunk_count; ++i) {
		int num = thunk[i] & 0xFFFF;
		int off = num * 8;
		thunk[i] = 0x81000000 | off;
		thunk_out[off++] = 0x68;
		thunk_out[off++] = num & 0xFF;
		thunk_out[off++] = (num >> 8) & 0xFF;
		thunk_out[off++] = 0;
		thunk_out[off++] = 0;
		thunk_out[off++] = 0xCD;
		thunk_out[off++] = 0x69;
	}
	cpu.map_pages(0x81000000, 0, thunk_count * 8);*/
	cpu.run(entry);
	
	free(mem);

	return 0;
}
