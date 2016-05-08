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
	header = (XbeHeader_t *) file_data;
	assert(header->soh == 0x1000);
	full_header = new uint8_t[header->soh];
	memcpy(full_header, file_data, header->soh);

	sections = new XbeSection_t[header->numsects];
	for(int i = 0; i < header->numsects; ++i)
		memcpy(&sections[i], &file_data[(header->secthdrs - header->base) + sizeof(XbeSection) * i], sizeof(XbeSection));

	header->oep ^= EPXORKEY;
	header->thunk ^= THXORKEY;
}

uint32_t Xbe::LoadImage() {
	cout << "Loading image at " << hex << header->base << " to " << hex << header->base + file_size << endl;
	cout << "OEP is " << hex << header->oep << endl;
	box->xbebase = header->base;
	box->pm->map(header->base, pagepad(file_size) / 4096);
	box->cpu->write_memory(header->base, file_size, file_data);

	uint32_t end = file_size;

	for(int i = 0; i < header->numsects; ++i) {
		XbeSection_t *sect = &sections[i];
		auto psize = pagepad(sect->vsize);
		auto base = sect->vaddr & ~0xFFF;
		if(sect->vaddr & 0xFFF)
			psize += 4096;
		box->pm->map(base, psize / 4096);
		cout << "Loading section of 0x" << hex << sect->vsize << " bytes (padded to 0x" << psize << ") to 0x" << base << endl;
		box->cpu->write_memory(sect->vaddr, sect->rsize, &file_data[sect->raddr]);
		uint32_t nend = (sect->vaddr - header->base) + sect->rsize;
		end = (nend > end) ? nend : end;
	}

	return pagepad(end);
}
