#include "Zookeeper.hpp"

IOManager::IOManager() {
	root = make_shared<Directory>();
	auto dirs = {
		"/Device", 
		"/Device/Harddisk0", 
		"D:", 
		"T:", 
		"U:", 
		"Z:"
	};
	for(auto dir : dirs)
		create_directory(dir);

	create_link("/Device/Harddisk0/partition1", "T:");
}

shared_ptr<IOHandle> IOManager::open(string path) {
	auto type = lookup_type(path);
	switch(type) {
		case IO_DIRECTORY:
			return lookup_directory(path)->open();
		case IO_FILE:
			return lookup_file(path)->open();
		case IO_UNKNOWN:
			cout << "Attempted to open unknown file/directory: " << path << endl;
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
			bailout(true);
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
		bailout(true);
	}
	return dir->files[fn];
}

shared_ptr<Directory> IOManager::create_directory(string path) {
	auto p = parse_path(path);
	auto dn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	
	auto ndir = make_shared<Directory>();
	dir->subdirectories[dn] = ndir;
	return ndir;
}

shared_ptr<File> IOManager::create_file(string path, function<shared_ptr<IOHandle>()> init) {
	auto p = parse_path(path);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	
	auto nf = make_shared<File>(init);
	dir->files[fn] = nf;
	return nf;
}

void IOManager::create_link(string from, string to) {
	auto p = parse_path(from);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);

	switch(lookup_type(to)) {
		case IO_DIRECTORY: {
			auto target = lookup_directory(to);
			dir->subdirectories[fn] = target;
			break;
		}
		case IO_FILE: {
			auto target = lookup_file(to);
			dir->files[fn] = target;
			break;
		}
		case IO_UNKNOWN:
			bailout("Could not find link target");
	}
}

IOHandle::IOHandle() {
	handle = box->io->create_handle();
}

File::File(function<shared_ptr<IOHandle>()> _init) {
	init = _init;
}

shared_ptr<IOHandle> File::open() {
	auto hnd = init();
	hnd->type = IOType::IO_FILE;
	hnd->path = path;
	return hnd;
}

shared_ptr<IOHandle> Directory::open() {
	auto hnd = static_pointer_cast<IOHandle>(make_shared<DirHandle>());
	hnd->type = IOType::IO_DIRECTORY;
	hnd->path = path;
	return hnd;
}

void FileHandle::read() {
}
void FileHandle::write() {
}
void FileHandle::close() {
}

void DirHandle::read() {
}
void DirHandle::write() {
}
void DirHandle::close() {
}
