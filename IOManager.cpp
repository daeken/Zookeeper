#include "Zookeeper.hpp"

uint32_t IOManager::open(string fn) {
	cout << "Attempting to open " << fn << endl;
	return 0;
}

uint32_t IOManager::add_handle(shared_ptr<IOHandle> handle) {
	assert(handle_id < 0xFFFFFFFF);
	handles[++handle_id] = handle;
	return handle_id;
}
