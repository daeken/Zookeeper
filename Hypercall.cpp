#include "Zookeeper.hpp"

void Hypercall::log_(uint32_t message) {
	string str = "";
	do {
		auto c = box->cpu->read_memory<uint8_t>(message++);
		if(c == 0)
			break;
		str += c;
	} while(true);
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

uint32_t Hypercall::get_xbebase() {
	return box->xbebase;
}

uint32_t Hypercall::query_eeprom(uint32_t index) {
	return 0;
}

