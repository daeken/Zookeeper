#pragma once
#include "Zookeeper.hpp"

class IOHandle {
public:
	virtual void read() = 0;
	virtual void write() = 0;
	virtual void close() = 0;
};

class FileHandle : IOHandle {
public:
	string filename;
};

class FSTree : map<string, unique_ptr<FSTree>> {
};

class DirectoryTree : FSTree {
public:

};

class FileEntry : FSTree {
public:
};

class IOManager {
public:
	uint32_t open(string fn);
	uint32_t add_handle(shared_ptr<IOHandle> handle);

	map<uint32_t, shared_ptr<IOHandle>> handles;
	uint32_t handle_id;
};
