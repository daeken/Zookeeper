#pragma once
#include "Zookeeper.hpp"

enum IOType {
	IO_UNKNOWN,
	IO_FILE,
	IO_DIRECTORY
};

class IOHandle {
public:
	IOHandle();
	virtual void read() = 0;
	virtual void write() = 0;
	virtual void close() = 0;

	uint32_t handle;
	IOType type;
	string path;
};

class FileHandle : public IOHandle {
public:
	void read();
	void write();
	void close();
};

class DirHandle : public IOHandle {
public:
	void read();
	void write();
	void close();
};

class File {
public:
	File(function<shared_ptr<IOHandle>()> _init);
	shared_ptr<IOHandle> open();

	string path;
	function<shared_ptr<IOHandle>()> init;
};

class Directory {
public:
	shared_ptr<IOHandle> open();

	map<string, shared_ptr<Directory>> subdirectories;
	map<string, shared_ptr<File>> files;
	string path;
};

class IOManager {
public:
	IOManager();
	shared_ptr<IOHandle> open(string fn);
	uint32_t create_handle();
	shared_ptr<IOHandle> get_handle(uint32_t handle_id);

	IOType lookup_type(string path);
	shared_ptr<File> lookup_file(string path);
	shared_ptr<Directory> lookup_directory(string path);
	shared_ptr<Directory> lookup_directory(list<string> path);

	shared_ptr<Directory> create_directory(string path);
	shared_ptr<File> create_file(string path, function<shared_ptr<IOHandle>()>);

	map<uint32_t, shared_ptr<IOHandle>> handles;
	uint32_t handle_id;
	shared_ptr<Directory> root;
};
