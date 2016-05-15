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
uint32_t Hypercall::map_aligned(uint32_t virt_base, uint32_t count) {
	return box->pm->map(virt_base, count, true);
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

uint32_t Hypercall::io_open(uint32_t dir_handle, uint32_t fn) {
	auto fnstr = read_string(fn);
	if(dir_handle != 0) {
		auto dirhnd = box->hm->get_handle<DirHandle>(dir_handle);
		assert(dirhnd->type == IOType::IO_DIRECTORY);
		fnstr = dirhnd->path + "\\" + fnstr;
	}
	auto file = box->io->open(fnstr);
	if(file == NULL)
		return 0;
	return file->handle;
}
