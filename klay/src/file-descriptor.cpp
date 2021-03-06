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

#include <unistd.h>

#include <klay/file-descriptor.h>

namespace klay {

FileDescriptor::~FileDescriptor()
{
	if ((fileDescriptor != -1) && (autoClose == true)) {
		::close(fileDescriptor);
	}
}

FileDescriptor& FileDescriptor::operator=(const int fd)
{
	fileDescriptor = fd;
	autoClose = false;
	return *this;
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& rhs)
{
	if (this != &rhs) {
		fileDescriptor = rhs.fileDescriptor;
		autoClose = rhs.autoClose;
		rhs.fileDescriptor = -1;
	}
	return *this;
}

} // namespace klay
