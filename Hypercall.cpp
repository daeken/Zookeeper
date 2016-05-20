#include "Zookeeper.hpp"

string read_string(uint32_t addr) {
	string str = "";
	do {
		auto c = box->cpu->read_memory<uint8_t>(addr++);
		if(c == 0)
			break;
		str += c;
	} while(true);
	return str;
}

void Hypercall::log_(uint32_t message) {
	auto str = read_string(message);
	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
	cout << "Guest Log: \"" << str << "\"" << endl;
}

uint32_t Hypercall::map(uint32_t virt_base, uint32_t count) {
	return box->pm->map(virt_base, count);
}
uint32_t Hypercall::map_contiguous(uint32_t virt_base, uint32_t phys_low, uint32_t phys_high, uint32_t count) {
	return box->pm->map_contiguous(virt_base, phys_low, phys_high, count);
}
uint32_t Hypercall::query_map_size(uint32_t base) {
	auto count = 0;
	while(box->cpu->is_mapped(base + count * PAGE_SIZE))
		count++;
	return count * PAGE_SIZE;
}
void Hypercall::unmap(uint32_t virt_base, uint32_t count) {
	box->pm->unmap(virt_base, count);
}

uint32_t Hypercall::create_thread(uint32_t eip, uint32_t esp, uint32_t arg) {
	esp -= 12;
	auto tid = box->tm->create(eip, esp);
	box->cpu->write_memory(esp + 4, tid);
	box->cpu->write_memory(esp + 8, arg);
	return tid;
}

void Hypercall::terminate_thread() {
	box->tm->terminate();
}

uint32_t Hypercall::get_xbebase() {
	return box->xbebase;
}

uint32_t Hypercall::query_eeprom(uint32_t index) {
	return 0;
}

uint32_t Hypercall::io_open(uint32_t dir_handle, uint32_t fn, FSFlags flags) {
	auto fnstr = read_string(fn);
	if(dir_handle != 0) {
		auto dirhnd = box->hm->get<DirHandle>(dir_handle);
		assert(dirhnd->type == IOType::IO_DIRECTORY);
		fnstr = dirhnd->path + "\\" + fnstr;
	}
	auto file = box->io->open(fnstr, flags);
	if(file == NULL)
		return 0;
	return file->handle;
}

bool Hypercall::io_read(uint32_t handle, uint32_t buf, uint32_t count, int64_t offset) {
	auto nbuf = new uint8_t[count];
	auto hnd = box->hm->get<IOHandle>(handle);
	hnd->read(nbuf, count, offset);
	box->cpu->write_memory(buf, count, nbuf);
	delete nbuf;
	return true;
}

bool Hypercall::io_write(uint32_t handle, uint32_t buf, uint32_t count, int64_t offset) {
	auto nbuf = new uint8_t[count];
	box->cpu->read_memory(buf, count, nbuf);
	auto hnd = box->hm->get<IOHandle>(handle);
	hnd->write(nbuf, count, offset);
	delete nbuf;
	return true;
}

bool Hypercall::io_ioctl(uint32_t handle, uint32_t code, uint32_t ibuf, uint32_t isize, uint32_t obuf, uint32_t osize) {
	auto irb = isize != 0 ? new uint8_t[isize] : NULL;
	auto orb = osize != 0 ? new uint8_t[osize] : NULL;
	if(orb) memset(orb, 0, osize);
	box->cpu->read_memory(ibuf, isize, irb);
	auto hnd = box->hm->get<IOHandle>(handle);
	hnd->ioctl(code, irb, isize, orb, osize);
	box->cpu->write_memory(obuf, osize, orb);
	if(isize != 0) delete irb;
	if(osize != 0) delete orb;
	return true;
}

void Hypercall::close(uint32_t handle) {
	box->hm->remove(handle);
}

void Hypercall::get_system_time(uint32_t addr) {
	timeval stime;
	gettimeofday(&stime, NULL);
	auto time = ((uint64_t) stime.tv_sec) * 10000000 + ((uint64_t) stime.tv_usec) * 10;
	box->cpu->write_memory<uint64_t>(addr, time);
}
