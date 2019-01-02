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
	klay::DirectoryIterator iter("/dev");
	klay::DirectoryIterator end;

	TEST_EXPECT(false, iter == end);

	while (iter != end) {
		++iter;
	}
}

TESTCASE(FileIO)
{
	char testbuf[100] = "Test Data";
	klay::File tmp("/tmp/test-file");
	try {
		tmp.create(0755);
		tmp.lock();
		tmp.write(testbuf, ::strlen(testbuf));
		tmp.lseek(10, SEEK_SET);
		tmp.unlock();
		tmp.close();
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}

	char readbuf[100];
	try {
		klay::File tmpFile("/tmp/test-file", O_RDWR);
		tmpFile.read(readbuf, ::strlen(testbuf));
		tmpFile.close();
		tmpFile.remove();
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(DirOperation)
{
	klay::File testDir("/tmp/dpm-unit-test/dir");
	try {
		testDir.makeDirectory(true, ::getuid(), ::getgid());
		testDir.chown(::getuid(), ::getgid(), false);
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}

	klay::File dir("/tmp/dpm-unit-test");
	try {
		dir.chmod(777, true);
		dir.remove(true);
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(FileAttribute)
{
	klay::File tmp("/tmp");

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
		klay::File tmp("/unknown");

		TEST_EXPECT(false, tmp.exists());
		TEST_EXPECT(false, tmp.canRead());
		TEST_EXPECT(false, tmp.canWrite());
		TEST_EXPECT(false, tmp.canExecute());

		try {
			tmp.isLink();
		} catch (klay::Exception& e) {
		}

		try {
			tmp.isFile();
		} catch (klay::Exception& e) {
		}

		try {
			tmp.isDirectory();
		} catch (klay::Exception& e) {
		}

		try {
			tmp.isDevice();
		} catch (klay::Exception& e) {
		}
	} catch (klay::Exception& e) {
	}
}

TESTCASE(FileDevice)
{
	klay::File tmp("/dev/kmem");

	TEST_EXPECT(true, tmp.isDevice());
}

TESTCASE(FileSymlinkTest)
{
	klay::File tmp("/var");

	TEST_EXPECT(true, tmp.isLink());
}

TESTCASE(FileReferenceTest)
{
	klay::File one("/tmp");
	klay::File two(one);
}
