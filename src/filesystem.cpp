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

#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/sendfile.h>

#include <string>
#include <sstream>
#include <iostream>

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/filesystem.h>

namespace runtime {

File::File(const std::string& pathname, int flags) :
	File(pathname)
{
	open(flags);
}

File::~File()
{
	close();
}

bool File::exists() const
{
	struct stat st;
	return (::stat(path.c_str(), &st) == 0);
}

bool File::canRead() const
{
	if (::access(path.c_str(), R_OK) == 0) {
		return true;
	}

	return false;
}

bool File::canWrite() const
{
	if (::access(path.c_str(), W_OK) == 0) {
		return true;
	}

	return false;
}

bool File::canExecute() const
{
	if (::access(path.c_str(), X_OK) == 0) {
		return true;
	}

	return false;
}

bool File::isLink() const
{
	struct stat st;
	if (::lstat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return S_ISLNK(st.st_mode);
}

bool File::isFile() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return S_ISREG(st.st_mode);
}

bool File::isDirectory() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return S_ISDIR(st.st_mode);
}

bool File::isDevice() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode));
}

mode_t File::getMode() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return st.st_mode;
}

uid_t File::getUid() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return st.st_uid;
}

gid_t File::getGid() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return st.st_gid;
}

size_t File::size() const
{
	struct stat st;
	if (::stat(path.c_str(), &st) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	return st.st_size;
}

void File::create(mode_t mode)
{
	while (1) {
		descriptor = ::creat(path.c_str(), mode);
		if (descriptor == -1) {
			if (errno == EINTR) {
				continue;
			}
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
		return;
	}
}

void File::open(int flags)
{
	while (1) {
		descriptor = ::open(path.c_str(), flags);
		if (descriptor == -1) {
			if (errno == EINTR) {
				continue;
			}
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
		return;
	}
}

void File::close()
{
	if (descriptor != -1) {
		while ((::close(descriptor) == -1) && (errno == EINTR));
		descriptor = -1;
	}
}

void File::read(void *buffer, const size_t size) const
{
	size_t total = 0;

	while (total < size) {
		int bytes = ::read(descriptor, reinterpret_cast<char*>(buffer) + total, size - total);
		if (bytes >= 0) {
			total += bytes;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		} else {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
	}
}

void File::write(const void *buffer, const size_t size) const
{
	size_t written = 0;

	while (written < size) {
		int bytes = ::write(descriptor, reinterpret_cast<const char*>(buffer) + written, size - written);
		if (bytes >= 0) {
			written += bytes;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			continue;
		} else {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
	}
}

void File::lseek(off_t offset, int whence) const
{
	if (::lseek(descriptor, offset, whence) == -1) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}
}

File File::copyTo(const std::string& destDir)
{
	const std::string& filename = getPath();
	File destFile(destDir);
	if (destFile.exists()) {
		destFile = destDir + "/" + getName();
	}

	if (isDirectory()) {
		DirectoryIterator iter(filename), end;
		destFile.makeDirectory(false, getUid(), getGid());

		while (iter != end) {
			iter->copyTo(destFile.getPath());
			++iter;
		}
	} else {
		open(O_RDONLY);
		destFile.create(getMode());
		::sendfile(destFile.descriptor, descriptor, 0, size());
		destFile.close();
		close();
	}

	return destFile;
}

void File::remove(bool recursive)
{
	if (isDirectory()) {
		if (recursive) {
			DirectoryIterator iter(path), end;
			while (iter != end) {
				iter->remove(true);
				++iter;
			}
		}
		if (::rmdir(path.c_str()) != 0) {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
	} else {
		if (::unlink(path.c_str()) != 0) {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
	}
}

void File::makeBaseDirectory(uid_t uid, gid_t gid)
{
	std::string::size_type i = path[0] != '/' ? -1 : 0;
	const std::string& pathStr = path;
	while ((i = pathStr.find('/', i + 1)) != std::string::npos) {
		std::string substr = pathStr.substr(0, i);
		int ret = ::mkdir(substr.c_str(), 0777);
		if ((ret == -1) && (errno == EEXIST)) {
			continue;
		} else if (ret == 0) {
			if ((uid | gid) != 0) {
				if (::chown(substr.c_str(), uid, gid) == -1) {
					throw runtime::Exception(runtime::GetSystemErrorMessage());
				}
			}
		} else {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}
	}
}

void File::makeDirectory(bool recursive, uid_t uid, gid_t gid)
{
	if (recursive) {
		makeBaseDirectory(uid, gid);
	}

	if (::mkdir(path.c_str(), 0777) == -1) {
		throw runtime::Exception("mkdir failed in makeDirectory: " + path);
	}

	if ((uid | gid) != 0) {
		chown(uid, gid);
	}
}

void File::chown(uid_t uid, gid_t gid, bool recursive)
{
	if (::chown(path.c_str(), uid, gid) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	if (recursive && isDirectory()) {
		DirectoryIterator iter(path), end;
		while (iter != end) {
			iter->chown(uid, gid, true);
			++iter;
		}
	}
}

void File::chmod(mode_t mode, bool recursive)
{
	if (::chmod(path.c_str(), mode) != 0) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	if (recursive && isDirectory()) {
		DirectoryIterator iter(path), end;
		while (iter != end) {
			iter->chmod(mode, true);
			++iter;
		}
	}
}

void File::lock() const
{
	if (::flock(descriptor, LOCK_EX) == -1) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}
}

void File::unlock() const
{
	if (::flock(descriptor, LOCK_UN) == -1) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}
}

DirectoryIterator::DirectoryIterator(const std::string& dir)
	: directoryHandle(nullptr)
{
	reset(dir);
}

DirectoryIterator::~DirectoryIterator()
{
	if (directoryHandle != nullptr) {
		::closedir(directoryHandle);
	}
}

void DirectoryIterator::reset(const std::string& dir)
{
	if (directoryHandle != nullptr) {
		::closedir(directoryHandle);
		directoryHandle = nullptr;
	}

	basename = dir;
	directoryHandle = ::opendir(basename.c_str());
	if (directoryHandle == nullptr) {
		throw runtime::Exception(runtime::GetSystemErrorMessage());
	}

	next();
}

void DirectoryIterator::next()
{
	std::string name;
	struct dirent entry, *ent;

	while (1) {
		if (readdir_r(directoryHandle, &entry, &ent) != 0) {
			throw runtime::Exception(runtime::GetSystemErrorMessage());
		}

		if (ent == NULL)
			break;

		if (ent->d_name[0] == '.' && ent->d_name[1] == '\0') {
			continue;
		}

		if (ent->d_name[0] == '.' &&
				ent->d_name[1] == '.' && ent->d_name[2] == '\0') {
			continue;
		}

		name = basename + "/" + std::string(ent->d_name);
		break;
	}

	current = name;
}

DirectoryIterator& DirectoryIterator::operator=(const std::string& dir)
{
	reset(dir);
	return *this;
}

DirectoryIterator& DirectoryIterator::operator++()
{
	next();
	return *this;
}

static const struct mountOption {
	const char* name;
	int clear;
	int flag;
} mountOptions[] = {
	{ "defaults",      0, 0                },
	{ "ro",            0, MS_RDONLY        },
	{ "rw",            1, MS_RDONLY        },
	{ "suid",          1, MS_NOSUID        },
	{ "nosuid",        0, MS_NOSUID        },
	{ "dev",           1, MS_NODEV         },
	{ "nodev",         0, MS_NODEV         },
	{ "exec",          1, MS_NOEXEC        },
	{ "noexec",        0, MS_NOEXEC        },
	{ "sync",          0, MS_SYNCHRONOUS   },
	{ "async",         1, MS_SYNCHRONOUS   },
	{ "dirsync",       0, MS_DIRSYNC       },
	{ "remount",       0, MS_REMOUNT       },
	{ "mand",          0, MS_MANDLOCK      },
	{ "nomand",        1, MS_MANDLOCK      },
	{ "atime",         1, MS_NOATIME       },
	{ "noatime",       0, MS_NOATIME       },
	{ "diratime",      1, MS_NODIRATIME    },
	{ "nodiratime",    0, MS_NODIRATIME    },
	{ "bind",          0, MS_BIND          },
	{ "rbind",         0, MS_BIND | MS_REC },
	{ "relatime",      0, MS_RELATIME      },
	{ "norelatime",    1, MS_RELATIME      },
	{ "strictatime",   0, MS_STRICTATIME   },
	{ "nostrictatime", 1, MS_STRICTATIME   },
	{ NULL,            0, 0                },
};

static unsigned long parseMountOptions(const std::string& opts, std::string& mntdata)
{
	std::stringstream optsSstream(opts);
	const struct mountOption* mo;
	unsigned long mntflags = 0L;
	std::string opt;

	while (std::getline(optsSstream, opt, ',')) {
		for (mo = mountOptions; mo->name != NULL; mo++) {
			if (opt == mo->name) {
				if (mo->clear) {
					mntflags &= ~mo->flag;
				} else {
					mntflags |= mo->flag;
				}
				break;
			}
			if (mo->name != NULL) {
				continue;
			}
			if (!mntdata.empty()) {
				mntdata += ",";
			}
			mntdata += opt;
		}
	}

	return mntflags;
}

void Mount::mountEntry(const std::string& src, const std::string& dest, const std::string& type, const std::string& opts)
{
	int ret;
	unsigned long mntflags;
	std::string mntdata;

	mntflags = parseMountOptions(opts, mntdata);

	ret = ::mount(src.c_str(),
				  dest.c_str(),
				  type.c_str(),
				  mntflags & ~MS_REMOUNT,
				  mntdata.c_str());

	if (ret) {
		if ((mntflags & MS_REMOUNT) || (mntflags & MS_BIND)) {
			ret = ::mount(src.c_str(),
						  dest.c_str(),
						  type.c_str(),
						  mntflags | MS_REMOUNT,
						  mntdata.c_str());
		}

		if (ret) {
			throw runtime::Exception("failed to mount " + src + " on " + dest);
		}
	}
}

} // namespace runtime
