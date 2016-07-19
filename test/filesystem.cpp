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
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <stdexcept>

#include <klay/exception.h>
#include <klay/filesystem.h>

#include <klay/testbench.h>

TESTCASE(DirectoryIteration)
{
	runtime::DirectoryIterator iter("/dev");
	runtime::DirectoryIterator end;

	TEST_EXPECT(false, iter == end);

	while (iter != end) {
		++iter;
	}
}

TESTCASE(FileIO)
{
	char testbuf[100] = "Test Data";
	runtime::File tmp("/tmp/test-file");
	try {
		tmp.create(755);
		tmp.lock();
		tmp.write(testbuf, ::strlen(testbuf));
		tmp.unlock();
		tmp.close();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}

	char readbuf[100];
	try {
		runtime::File tmpFile("/tmp/test-file", O_RDWR);
		tmpFile.read(readbuf, ::strlen(testbuf));
		tmpFile.close();
		tmpFile.remove();
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(DirOperation)
{
	runtime::File testDir("/tmp/dpm-unit-test/dir");
	try {
		testDir.makeDirectory(true, ::getuid(), ::getgid());
		testDir.chown(::getuid(), ::getgid(), false);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}

	runtime::File dir("/tmp/dpm-unit-test");
	try {
		dir.chmod(777, true);
		dir.remove(true);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(FileAttribute)
{
	runtime::File tmp("/tmp");

	TEST_EXPECT(true, tmp.exists());
	TEST_EXPECT(true, tmp.canRead());
	TEST_EXPECT(true, tmp.canWrite());
	TEST_EXPECT(true, tmp.canExecute());
	TEST_EXPECT(false, tmp.isLink());
	TEST_EXPECT(false, tmp.isFile());
	TEST_EXPECT(true, tmp.isDirectory());
	TEST_EXPECT(false, tmp.isDevice());

	std::cout << " UID: " << tmp.getUid()
			  << " GID: " << tmp.getGid()
			  << " Size: " << tmp.size()
			  << " Mode: " << tmp.getMode()
			  << " Path: " << tmp.getPath()
			  << " File: " << tmp.getName() << std::endl;
}

TESTCASE(FileAttributeNegative)
{
	try {
		runtime::File tmp("/unknown");

		TEST_EXPECT(false, tmp.exists());
		TEST_EXPECT(false, tmp.canRead());
		TEST_EXPECT(false, tmp.canWrite());
		TEST_EXPECT(false, tmp.canExecute());

		try {
			tmp.isLink();
		} catch (runtime::Exception& e) {
		}

		try {
			tmp.isFile();
		} catch (runtime::Exception& e) {
		}

		try {
			tmp.isDirectory();
		} catch (runtime::Exception& e) {
		}

		try {
			tmp.isDevice();
		} catch (runtime::Exception& e) {
		}
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(FileDevice)
{
	runtime::File tmp("/dev/kmem");

	TEST_EXPECT(true, tmp.isDevice());
}

TESTCASE(FileSymlinkTest)
{
	runtime::File tmp("/var");

	TEST_EXPECT(true, tmp.isLink());
}

TESTCASE(FileReferenceTest)
{
	runtime::File one("/tmp");
	runtime::File two(one);
}
