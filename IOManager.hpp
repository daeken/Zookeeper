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

class File {
public:
};

class Directory {
public:
	map<string, shared_ptr<Directory>> subdirectories;
	map<string, shared_ptr<File>> files;
};

class IOManager {
public:
	IOManager();
	uint32_t open(string fn);
	uint32_t add_handle(shared_ptr<IOHandle> handle);
	shared_ptr<File> lookup_file(string path);
	shared_ptr<Directory> lookup_directory(string path);
	shared_ptr<Directory> lookup_directory(list<string> path);

	shared_ptr<Directory> create_directory(string path);
	shared_ptr<File> create_file(string path);

	map<uint32_t, shared_ptr<IOHandle>> handles;
	uint32_t handle_id;
	shared_ptr<Directory> root;
};
