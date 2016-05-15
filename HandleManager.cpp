#include "Zookeeper.hpp"

void Handle::close() {
	box->hm->remove(handle);
}

uint32_t HandleManager::add(shared_ptr<Handle> handle) {
	bailout(handle_id == 0xFFFFFFFE); // XXX: We should have a way of recycling handles
	handle->handle = ++handle_id;
	handles[handle_id] = handle;
	return handle_id;
}

shared_ptr<Handle> HandleManager::get(uint32_t handle_id) {
	return handles[handle_id];
}

void HandleManager::remove(uint32_t handle_id) {
	auto iter = handles.find(handle_id);
	if(iter != handles.end())
		handles.erase(iter);
}
