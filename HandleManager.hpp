#pragma once
#include "Zookeeper.hpp"

class Handle {
public:
	virtual void close();
	uint32_t handle;
};

class HandleManager {
public:
	uint32_t add(shared_ptr<Handle> handle);
	shared_ptr<Handle> get(uint32_t handle_id);
	void remove(uint32_t handle_id);

	template<typename T>
	uint32_t add(shared_ptr<T> handle) {
		return add(static_pointer_cast<Handle>(handle));
	}
	template<typename T>
	shared_ptr<T> get(uint32_t handle_id) {
		return dynamic_pointer_cast<T>(get(handle_id));
	}

	map<uint32_t, shared_ptr<Handle>> handles;
	uint32_t handle_id;
};
