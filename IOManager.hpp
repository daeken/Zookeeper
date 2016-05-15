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
	IOType type;
	string path;
};

class FileHandle : public IOHandle {
public:
	FileHandle(string path, string mapped_path);
	void read();
	void write();
	void close();

	string mapped_path;
};

class DirHandle : public IOHandle {
public:
	DirHandle(string path);
	void close();
};

class Directory {
public:
	shared_ptr<IOHandle> open();

	map<string, shared_ptr<Directory>> subdirectories;
	map<string, string> dirmaps;
	string path;
};

class IOManager {
public:
	IOManager();
	shared_ptr<IOHandle> open(string fn);
	shared_ptr<IOHandle> open_file(string path);

	shared_ptr<Directory> lookup_directory(string path);
	shared_ptr<Directory> lookup_directory(list<string> path);
	string lookup_map(string path);

	shared_ptr<Directory> create_directory(string path);
	void create_link(string from, string to);
	void create_map(string from, string to);

	shared_ptr<Directory> root;
};
