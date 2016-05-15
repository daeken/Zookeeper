#include "Zookeeper.hpp"

uint32_t HandleManager::add_handle(shared_ptr<Handle> handle) {
	handle->handle = ++handle_id;
	handles[handle_id] = handle;
	return handle_id;
}

shared_ptr<Handle> HandleManager::get_handle(uint32_t handle_id) {
	return handles[handle_id];
}
