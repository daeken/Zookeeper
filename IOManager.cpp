#include "Zookeeper.hpp"

IOManager::IOManager() {
	root = make_shared<Directory>();
	auto dirs = {
		"/Device", 
		"/Device/Harddisk0"
		"/D:", 
		"/T:", 
		"/U:", 
		"/Z:"
	};
	for(auto dir : dirs)
		create_directory(dir);
}

shared_ptr<IOHandle> IOManager::open(string path) {
	auto type = lookup_type(path);
	switch(type) {
		case IO_DIRECTORY:
			return static_pointer_cast<IOHandle>(lookup_directory(path)->open());
		case IO_FILE:
			return static_pointer_cast<IOHandle>(lookup_file(path)->open());
		case IO_VOLUME:
			return NULL;
		case IO_UNKNOWN:
			return NULL;
	}
}

uint32_t IOManager::create_handle() {
	assert(handle_id < 0xFFFFFFFF);
	return ++handle_id;
}

shared_ptr<IOHandle> IOManager::get_handle(uint32_t handle_id) {
	return handles[handle_id];
}

list<string> parse_path(string path) {
	list<string> vec;
	to_lower(path);
	split(vec, path, boost::is_any_of("\\/"));
	if(vec.back() == "") // In case of trailing slash on directories
		vec.pop_back();
	if(vec.front() == "") // In case of starting slash
		vec.pop_front();
	return vec;
}

IOType IOManager::lookup_type(string path) {
	auto p = parse_path(path);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	if(dir == NULL)
		return IOType::IO_UNKNOWN;

	if(dir->subdirectories.find(fn) != dir->subdirectories.end())
		return IOType::IO_DIRECTORY;
	else if(dir->files.find(fn) != dir->files.end())
		return IOType::IO_FILE;
	return IOType::IO_UNKNOWN;
}

shared_ptr<Directory> IOManager::lookup_directory(string path) {
	return lookup_directory(parse_path(path));
}

shared_ptr<Directory> IOManager::lookup_directory(list<string> path) {
	auto dir = root;
	for(auto e : path) {
		if(dir->subdirectories.find(e) == dir->subdirectories.end()) {
			cout << "Could not find " << e << " in path " << join(path, "\\") << endl;
			return NULL;
		}

		dir = dir->subdirectories[e];
	}
	return dir;
}

shared_ptr<File> IOManager::lookup_file(string path) {
	auto p = parse_path(path);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	if(dir == NULL)
		return NULL;

	if(dir->files.find(fn) == dir->files.end()) {
		cout << "Could not find file " << path << endl;
		return NULL;
	}
	return dir->files[fn];
}

shared_ptr<Directory> IOManager::create_directory(string path) {
	auto p = parse_path(path);
	auto dn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	if(dir == NULL)
		return NULL;

	auto ndir = make_shared<Directory>();
	dir->subdirectories[dn] = ndir;
	return ndir;
}

IOHandle::IOHandle() {
	handle = box->io->create_handle();
}

shared_ptr<FileHandle> File::open() {
	return NULL;
}

shared_ptr<DirHandle> Directory::open() {
	return NULL;
}
