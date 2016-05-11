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
};

class IOManager {
public:
	uint32_t open(string fn);
};
