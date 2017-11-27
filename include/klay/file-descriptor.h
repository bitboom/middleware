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

#ifndef __RUNTIME_FILE_DESCRIPTOR_H__
#define __RUNTIME_FILE_DESCRIPTOR_H__

#include <klay/klay.h>

namespace runtime {

struct KLAY_EXPORT FileDescriptor {
	FileDescriptor(int fd = -1, bool autoclose = false) :
		fileDescriptor(fd), autoClose(autoclose)
	{
	}

	FileDescriptor(const FileDescriptor&) = delete;
	FileDescriptor(FileDescriptor&& rhs) :
		fileDescriptor(rhs.fileDescriptor),
		autoClose(rhs.autoClose)
	{
		rhs.fileDescriptor = -1;
	}

	~FileDescriptor();

	FileDescriptor& operator=(const int fd);
	FileDescriptor& operator=(FileDescriptor&& rhs);

	int fileDescriptor;

private:
	bool autoClose;
};

}

#endif //__RUNTIME_FILE_DESCRIPTOR_H__
