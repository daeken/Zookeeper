#pragma once
#include "Zookeeper.hpp"

enum IOType {
	IO_UNKNOWN,
	IO_FILE,
	IO_DIRECTORY
};

class IOHandle : public Handle {
public:
	IOHandle(IOType type, string path);
	virtual void read(void *buf, uint32_t count, int64_t offset) = 0;
	virtual void write(void *buf, uint32_t count, int64_t offset) = 0;
	virtual void ioctl(uint32_t code, void *ibuf, uint32_t isize, void *obuf, uint32_t osize) = 0;
	virtual void close() = 0;
	IOType type;
	string path;
};

class FileHandle : public IOHandle {
public:
	FileHandle(string path, string mapped_path, FSFlags flags);
	void read(void *buf, uint32_t count, int64_t offset);
	void write(void *buf, uint32_t count, int64_t offset);
	void ioctl(uint32_t code, void *ibuf, uint32_t isize, void *obuf, uint32_t osize);
	void close();

	string mapped_path;
	bool writable;
	int fd;
};

class DirHandle : public IOHandle {
public:
	DirHandle(string path, string mapped_path, FSFlags flags);
	void read(void *buf, uint32_t count, int64_t offset);
	void write(void *buf, uint32_t count, int64_t offset);
	void ioctl(uint32_t code, void *ibuf, uint32_t isize, void *obuf, uint32_t osize);
	void close();
};

class Directory {
public:
	Directory(string path);
	shared_ptr<IOHandle> open();

	map<string, shared_ptr<Directory>> subdirectories;
	map<string, string> dirmaps;
	string path;
};

class IOManager {
public:
	IOManager();
	shared_ptr<IOHandle> open(string fn, FSFlags flags);

	shared_ptr<Directory> lookup_directory(string path);
	shared_ptr<Directory> lookup_directory(list<string> path);
	string lookup_map(string path);

	shared_ptr<Directory> create_directory(string path);
	void create_link(string from, string to);
	void create_map(string from, string to);

	shared_ptr<Directory> root;
};
