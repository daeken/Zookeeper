#include "Zookeeper.hpp"

uint32_t load_multiboot(Cpu *cpu, uint32_t *header) {
	assert(header[1] & 0x10000);

	uint8_t *rel = (uint8_t *) header;

	uint32_t memsize = header[6] - header[3];
	if(memsize & 0xFFF)
		memsize = (memsize & ~0xFFF) + 0x1000;
	assert(memsize <= KRAM_SIZE);

	memcpy(cpu->kmem + (header[4] - 0xC0000000), rel + (header[4] - header[3]), memsize);
	memset(cpu->kmem + (header[5] - 0xC0000000), 0, header[6] - header[5]);
	cpu->map_pages(0xC0000000, 0xC0000000, memsize / 4096);

	return header[7];
}

uint32_t load_kernel(Cpu *cpu) {
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
	box = new Box;

	uint32_t entry = load_kernel(box->cpu);
	auto xbe = new Xbe((char *) "test1.xbe");
	xbe->LoadImage();
	
	box->cpu->run(entry);
	
	return 0;
}
