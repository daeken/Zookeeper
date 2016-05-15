#pragma once
#include "Zookeeper.hpp"

class Handle {
public:
	virtual void close() = 0;
	uint32_t handle;
};

class HandleManager {
public:
	uint32_t add_handle(shared_ptr<Handle> handle);
	shared_ptr<Handle> get_handle(uint32_t handle_id);

	template<typename T>
	uint32_t add_handle(shared_ptr<T> handle) {
		return add_handle(static_pointer_cast<Handle>(handle));
	}
	template<typename T>
	shared_ptr<T> get_handle(uint32_t handle_id) {
		return dynamic_pointer_cast<T>(get_handle(handle_id));
	}

	map<uint32_t, shared_ptr<Handle>> handles;
	uint32_t handle_id;
};
