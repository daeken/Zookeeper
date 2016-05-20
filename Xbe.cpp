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
	for(auto i = 0; i < header->numsects; ++i)
		memcpy(&sections[i], &file_data[(header->secthdrs - header->base) + sizeof(XbeSection) * i], sizeof(XbeSection));

	header->oep ^= EPXORKEY;
	header->thunk ^= THXORKEY;
}

uint32_t Xbe::LoadImage() {
	//cout << "Loading image at " << hex << header->base << " to " << hex << header->base + file_size << endl;
	//cout << "OEP is " << hex << header->oep << endl;
	box->xbebase = header->base;
	box->pm->map(header->base, pagepad(file_size) / PAGE_SIZE);
	box->cpu->write_memory(header->base, file_size, file_data);

	auto end = file_size;

	for(auto i = 0; i < header->numsects; ++i) {
		XbeSection_t *sect = &sections[i];
		auto psize = pagepad(sect->vsize);
		auto base = sect->vaddr & ~PAGE_MASK;
		if(sect->vaddr & PAGE_MASK)
			psize += PAGE_SIZE;
		box->pm->map(base, psize / PAGE_SIZE);
		//cout << "Loading section of 0x" << hex << sect->vsize << " bytes (padded to 0x" << psize << ") to 0x" << base << endl;
		box->cpu->write_memory(sect->vaddr, sect->rsize, &file_data[sect->raddr]);
		auto nend = (sect->vaddr - header->base) + sect->rsize;
		end = (nend > end) ? nend : end;
	}

	return pagepad(end);
}
