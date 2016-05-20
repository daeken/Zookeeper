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
	root = make_shared<Directory>("/");
	auto dirs = {
		"/Device", 
		"/Device/CdRom0", 
		"/Device/Harddisk0", 
		"/Device/Harddisk0/partition0", 
		"/Device/Harddisk0/partition1", 
		"/Device/Harddisk0/partition3", 
	};
	for(auto dir : dirs)
		create_directory(dir);

	create_map("/Device/Harddisk0/partition1/TDATA", "fs/tdata");
	create_map("/Device/Harddisk0/partition1/UDATA", "fs/udata");

	create_link("D:", "/Device/CdRom0");
	create_link("T:", "/Device/Harddisk0/partition1/TDATA");
	create_link("U:", "/Device/Harddisk0/partition1/UDATA");
}

shared_ptr<IOHandle> IOManager::open(string pathstr, FSFlags flags) {
	auto path = parse_path(pathstr);
	auto dir = lookup_directory(pathstr);
	if(dir != NULL) {
		return dir->open();
	}

	auto mapped = lookup_map(pathstr);
	if(mapped == "")
		return NULL;

	shared_ptr<IOHandle> hnd;
	if(FLAG(flags, FSFlags::DIRECTORY))
		hnd = static_pointer_cast<IOHandle>(make_shared<DirHandle>(pathstr, mapped, flags));
	else
		hnd = static_pointer_cast<IOHandle>(make_shared<FileHandle>(pathstr, mapped, flags));
	box->hm->add(hnd);
	return hnd;
}

shared_ptr<Directory> IOManager::lookup_directory(string path) {
	return lookup_directory(parse_path(path));
}

shared_ptr<Directory> IOManager::lookup_directory(list<string> path) {
	auto dir = root;
	for(auto e : path) {
		if(!IN(e, dir->subdirectories)) {
			//cout << "Could not find " << e << " in path " << join(path, "\\") << endl;
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
		if(IN(e, dir->subdirectories))
			dir = dir->subdirectories[e];
		else if(IN(e, dir->dirmaps)) {
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
			return "";
		}
	}
	return "";
}

shared_ptr<Directory> IOManager::create_directory(string path) {
	auto p = parse_path(path);
	auto dn = p.back();
	p.pop_back();
	auto dir = lookup_directory(p);
	
	auto ndir = make_shared<Directory>(path);
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

Directory::Directory(string path) : path(path) {
}

shared_ptr<IOHandle> Directory::open() {
	auto hnd = static_pointer_cast<IOHandle>(make_shared<DirHandle>(path, "", FSFlags::READ | FSFlags::DIRECTORY));
	box->hm->add(hnd);
	return hnd;
}

IOHandle::IOHandle(IOType type, string path) : type(type), path(path) {
}

FileHandle::FileHandle(string path, string mapped_path, FSFlags flags) : IOHandle(IOType::IO_FILE, path), mapped_path(mapped_path) {
	cout << "Creating file handle for virtual path " << path << " to real path " << mapped_path << endl;
	writable = FLAG(flags, FSFlags::WRITE);
	fd = open(mapped_path.c_str(), (writable ? O_RDWR : O_RDONLY) | (FLAG(flags, FSFlags::CREATE) ? O_CREAT : 0));
	cout << "FD " << dec << fd << endl;
	bailout(fd == -1);
}
void FileHandle::read(void *buf, uint32_t count, int64_t offset) {
	bailout(offset != -1);
	::read(fd, buf, count);
}
void FileHandle::write(void *buf, uint32_t count, int64_t offset) {
	bailout(offset != -1);
	::write(fd, buf, count);
}
void FileHandle::ioctl(uint32_t code, void *ibuf, uint32_t isize, void *obuf, uint32_t osize) {
	cout << "Attempted ioctl on file " << path << endl;
	cout << format("Code: %08x") % code << endl;
}
void FileHandle::close() {
}

DirHandle::DirHandle(string path, string mapped_path, FSFlags flags) : IOHandle(IOType::IO_DIRECTORY, path) {
	if(mapped_path != "" && FLAG(flags, FSFlags::CREATE))
		mkdir(mapped_path.c_str(), 0700);
}
void DirHandle::read(void *buf, uint32_t count, int64_t offset) {
	//cout << "Attempting to read " << count << " bytes from directory " << path << endl;
	//cout << "Offset " << hex << offset << endl;
}
void DirHandle::write(void *buf, uint32_t count, int64_t offset) {
	if(count == 0) {
		cout << "Attempting to write " << count << " bytes to directory " << path << endl;
		cout << "Offset " << hex << offset << endl;
		cout << "Null write" << endl;
		box->cpu->break_in = true;
	}
}
void DirHandle::ioctl(uint32_t code, void *ibuf, uint32_t isize, void *obuf, uint32_t osize) {
	switch(code) {
		case IOCTL_DISK_GET_DRIVE_GEOMETRY: {
			auto geom = (DISK_GEOMETRY *) obuf;
			geom->BytesPerSector = ONE_MB;
			break;
		}
		case IOCTL_DISK_GET_PARTITION_INFO: {
			auto part = (PARTITION_INFORMATION *) obuf;
			part->StartingOffset = 0;
			part->PartitionLength = ONE_GB;
			part->HiddenSectors = 0;
			part->PartitionNumber = 5; // Who cares?
			part->PartitionType = 8;
			part->BootIndicator = 1;
			part->RecognizedPartition = 1;
			part->RewritePartition = 0;
			break;
		}
		default:
			cout << "Attempted unknown ioctl on dir " << path << endl;
			cout << format("Code: %08x") % code << endl;
			break;
	}
}
void DirHandle::close() {
}
