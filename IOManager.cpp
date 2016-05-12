#include "Zookeeper.hpp"

IOManager::IOManager() {
	root = make_shared<Directory>();
}

uint32_t IOManager::open(string fn) {
	cout << "Attempting to open " << fn << endl;
	return 0;
}

uint32_t IOManager::add_handle(shared_ptr<IOHandle> handle) {
	assert(handle_id < 0xFFFFFFFF);
	handles[++handle_id] = handle;
	return handle_id;
}

list<string> parse_path(string path) {
	list<string> vec;
	split(vec, path, boost::is_any_of("\\/"));
	if(vec.back() == "") // In case of trailing slash on directories
		vec.pop_back();
	if(vec.front() == "") // In case of starting slash
		vec.pop_front();
	return vec;
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
