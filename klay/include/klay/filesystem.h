/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#ifndef __RUNTIME_FILESYSTEM_H__
#define __RUNTIME_FILESYSTEM_H__

#include <dirent.h>
#include <sys/types.h>

#include <string>

#include <klay/klay.h>

namespace klay {

class KLAY_EXPORT File {
public:
	File() :
		descriptor(-1)
	{
	}

	File(const char* pathname) :
		descriptor(-1), path(pathname)
	{
	}

	File(const std::string& pathname) :
		descriptor(-1), path(pathname)
	{
	}

	File(const File& file) :
		File(file.getPath())
	{
	}

	File(const std::string& path, int flags);

	~File();

	File& operator=(const std::string& pathname)
	{
		path = pathname;
		return *this;
	}

	File& operator=(const File& file)
	{
		path = file.path;
		return *this;
	}

	bool operator<(const File& file) const;
	bool operator>(const File& file) const;
	bool operator<=(const File& file) const;
	bool operator>=(const File& file) const;

	bool operator==(const File& file) const
	{
		return (path == file.path);
	}

	bool operator!=(const File& file) const
	{
		return !(path == file.path);
	}

	bool exists() const;
	bool canRead() const;
	bool canWrite() const;
	bool canExecute() const;

	bool isLink() const;
	bool isFile() const;
	bool isDirectory() const;
	bool isDevice() const;

	mode_t getMode() const;
	uid_t getUid() const;
	gid_t getGid() const;
	ino_t getInode() const;
	dev_t getDevice() const;

	off_t size() const;

	void create(mode_t mode);
	void create(int flags, mode_t mode);
	void open(int flags);
	void read(void *buffer, const size_t size) const;
	void write(const void *buffer, const size_t size) const;
	void lseek(off_t offset, int whence) const;
	void close();
	File copyTo(const std::string& pathname);
	void remove(bool recursive = false);
	void makeBaseDirectory(uid_t uid = 0, gid_t gid = 0);
	void makeDirectory(bool recursive = false, uid_t uid = 0, gid_t gid = 0);

	void lock() const;
	void unlock() const;

	void chown(uid_t uid, gid_t gid, bool recursive = false);
	void chmod(mode_t mode, bool recursive = false);

	const std::string readlink() const;

	const std::string& getPath() const
	{
		return path;
	}

	const std::string getName() const
	{
		return path.substr(path.rfind('/') + 1);
	}

private:
	int descriptor;
	std::string path;
};

class KLAY_EXPORT DirectoryIterator {
public:
	DirectoryIterator() :
	    directoryHandle(nullptr)
	{
	}

	DirectoryIterator(const std::string& dir);

	~DirectoryIterator();

	DirectoryIterator& operator=(const std::string& dir);
	DirectoryIterator& operator++();

	bool operator==(const DirectoryIterator& iterator) const
	{
		return (current == iterator.current);
	}

	bool operator!=(const DirectoryIterator& iterator) const
	{
		return !(current == iterator.current);
	}

	const File& operator*() const
	{
		return current;
	}

	File& operator*()
	{
		return current;
	}

	const File* operator->() const
	{
		return &current;
	}

	File* operator->()
	{
		return &current;
	}

private:
	void next();
	void reset(const std::string& dir);

	File current;
	DIR* directoryHandle;
	std::string basename;
};

class KLAY_EXPORT Mount final {
public:
	Mount() = delete;

	static void mountEntry(const std::string& src, const std::string& dest,
						   const std::string& type, const std::string& opts);
};

int Open(const std::string& path, int flags, mode_t mode);
void Close(int fd);

} // namespace klay

namespace runtime = klay;

#endif //__RUNTIME_FILESYSTEM_H__
