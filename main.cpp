#include "Zookeeper.hpp"

int main(int argc, char **argv) {
	if(argc != 2) {
		cout << "Usage: " << argv[0] << " <binary.xbe>" << endl;
		return 0;
	}

	uint8_t *mem;
	bailout(!(mem = (uint8_t *) valloc(RAM_SIZE)));
	
	Xbe xbe(argv[1]);
	uint32_t size = xbe.LoadImage(((uint8_t *) mem) + 0x80000);

	Kernel kernel;

	Cpu cpu(mem, &kernel);
	cpu.map_pages(xbe.header.base, 0x80000, size / 4096);
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
	cpu.map_pages(0x81000000, 0, thunk_count * 8);
	cpu.run(xbe.header.oep);
	
	free(mem);

	return 0;
}
