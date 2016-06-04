#include "Zookeeper.hpp"

uint32_t load_multiboot(uint32_t *header) {
	assert(header[1] & 0x10000);

	uint8_t *rel = (uint8_t *) header;

	uint32_t memsize = header[6] - KBASE;
	if(memsize & PAGE_MASK)
		memsize = (memsize & ~PAGE_MASK) + PAGE_SIZE;
	assert(memsize <= KRAM_SIZE);

	memcpy(box->cpu->kmem + (header[4] - KBASE), rel + (header[4] - header[3]), memsize);
	memset(box->cpu->kmem + (header[5] - KBASE), 0, header[6] - header[5]);
	box->cpu->map_pages(KBASE, KBASE, memsize / PAGE_SIZE);

	return header[7];
}

uint32_t load_kernel() {
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
			if((entry = load_multiboot(seek)) != -1)
				break;
		}
	} while(++seek != end);

	if(seek == end) {
		cout << "Reached EOF before finding Multiboot header!" << endl;
	}

	delete[] buf;
	return entry;
}

void intHandler(int _) {
	break_in();
}

int main(int argc, char **argv) {
	new Box;

	uint32_t entry = load_kernel();
	auto xbe = new Xbe((char *) "test1.xbe");
	xbe->LoadImage();
	
	signal(SIGINT, intHandler);

	box->cpu->run(entry);

	delete box;
	
	return 0;
}
