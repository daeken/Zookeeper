#include "Zookeeper.hpp"

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

IOManager::IOManager() {
	root = make_shared<Directory>();
	auto dirs = {
		"/Device", 
		"/Device/CdRom0", 
		"/Device/Harddisk0", 
		"/Device/Harddisk0/partition0", 
		"/Device/Harddisk0/partition1", 
	};
	for(auto dir : dirs)
		create_directory(dir);

	create_map("/Device/Harddisk0/partition1/TDATA", "fs/tdata");
	create_map("/Device/Harddisk0/partition1/UDATA", "fs/udata");

	create_link("D:", "/Device/CdRom0");
	create_link("T:", "/Device/Harddisk0/partition1/TDATA");
	create_link("U:", "/Device/Harddisk0/partition1/UDATA");
}

shared_ptr<IOHandle> IOManager::open(string pathstr) {
	auto path = parse_path(pathstr);
	auto dir = lookup_directory(pathstr);
	if(dir != NULL)
		return dir->open();

	auto mapped = lookup_map(pathstr);
	auto file = static_pointer_cast<IOHandle>(make_shared<FileHandle>(pathstr, mapped));
	box->hm->add(file);
	return file;
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

string IOManager::lookup_map(string pathstr) {
	auto path = parse_path(pathstr);
	auto dir = root;
	auto i = 0;
	for(auto e : path) {
		++i;
		if(dir->subdirectories.find(e) != dir->subdirectories.end())
			dir = dir->subdirectories[e];
		else if(dir->dirmaps.find(e) != dir->dirmaps.end()) {
			auto base = dir->dirmaps[e];
			if(i == path.size())
				return base;
			else {
				auto bi = next(path.begin(), i);
				list<string> sub(bi, path.end());
				return base + "/" + join(sub, "/");
			}
		} else {
			cout << "Could not find " << e << " in path " << pathstr << endl;
			return NULL;
		}
	}
	return NULL;
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

void IOManager::create_link(string from, string to) {
	auto p = parse_path(from);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);

	auto target = lookup_directory(to);
	if(target != NULL) {
		dir->subdirectories[fn] = target;
		return;
	}
	auto target_map = lookup_map(to);
	dir->dirmaps[fn] = target_map;
}

void IOManager::create_map(string from, string to) {
	auto p = parse_path(from);
	auto fn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	dir->dirmaps[fn] = to;
}

shared_ptr<IOHandle> Directory::open() {
	auto hnd = static_pointer_cast<IOHandle>(make_shared<DirHandle>(path));
	box->hm->add(hnd);
	return hnd;
}

IOHandle::IOHandle(IOType type, string path) : type(type), path(path) {
}

FileHandle::FileHandle(string path, string mapped_path) : IOHandle(IOType::IO_FILE, path), mapped_path(mapped_path) {
	cout << "Creating file handle for virtual path " << path << " to real path " << mapped_path << endl;
}
void FileHandle::read() {
}
void FileHandle::write() {
}
void FileHandle::close() {
}

DirHandle::DirHandle(string path) : IOHandle(IOType::IO_DIRECTORY, path) {
}
void DirHandle::close() {
}
