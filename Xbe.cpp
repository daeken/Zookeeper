#include <cassert>
#include "Xbe.hpp"

#define EPXORKEY 0x94859D4B
#define THXORKEY 0xEFB1F152

Xbe::Xbe(char *fn) {
	fp = fopen(fn, "rb");
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	file_data = new uint8_t[file_size];
	fread(file_data, file_size, 1, fp);
	memcpy(&header, file_data, sizeof(XbeHeader));
	assert(header.soh == 0x1000);
	full_header = new uint8_t[header.soh];
	memcpy(full_header, file_data, header.soh);

	sections = new XbeSection[header.numsects];
	for(int i = 0; i < header.numsects; ++i)
		memcpy(&sections[i], &file_data[(header.secthdrs - header.base) + sizeof(XbeSection) * i], sizeof(XbeSection));

	header.oep ^= EPXORKEY;
	header.thunk ^= THXORKEY;
}

uint32_t Xbe::LoadImage(void *mem_base) {
	cout << "Loading image at " << hex << header.base << " to " << hex << header.base + file_size << endl;
	cout << "OEP is " << hex << header.oep << endl;
	memcpy((uint8_t *) mem_base, file_data, file_size);

	uint32_t end = file_size;

	for(int i = 0; i < header.numsects; ++i) {
		XbeSection *sect = &sections[i];
		memcpy((uint8_t *) mem_base + (sect->vaddr - header.base), &file_data[sect->raddr], sect->rsize);
		uint32_t nend = (sect->vaddr - header.base) + sect->rsize;
		end = (nend > end) ? nend : end;
	}
	if(end & 0xFFF)
		end = (end & ~0xFFF) + 4096;

	return end;
}
