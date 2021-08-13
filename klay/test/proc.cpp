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

#include <string>
#include <vector>

#include <klay/process.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>

#include <klay/testbench.h>

TESTCASE(ProcWithArg)
{
	try {
		std::vector<std::string> args = {
			"-l",
			"-a"
		};
		klay::Process proc("/bin/ls > /dev/null", args);
		TEST_EXPECT(true, proc.execute() != -1);
		proc.waitForFinished();
	} catch (klay::Exception& e) {
	}
}

TESTCASE(ProcKill)
{
	try {
		klay::Process proc("/opt/data/unittest-proc.sh");
		TEST_EXPECT(true, proc.execute() != -1);
		if (proc.isRunning()) {
			proc.kill();
			proc.waitForFinished();
		}
	} catch (klay::Exception& e) {
	}
}

TESTCASE(ProcTerminate)
{
	try {
		klay::Process proc("/opt/data/unittest-proc.sh");
		TEST_EXPECT(true, proc.execute() != -1);
		if (proc.isRunning()) {
			proc.terminate();
			proc.waitForFinished();
		}
	} catch (klay::Exception& e) {
	}
}

TESTCASE(ProcInvalidProcess)
{
	try {
		klay::Process proc(TEST_DATA_DIR "test-proc.sh");
		TEST_EXPECT(true, proc.execute() != -1);
		proc.terminate();
		proc.waitForFinished();
		TEST_EXPECT(false, proc.isRunning());
		try {
			proc.kill();
		} catch (klay::Exception& e) {
		}

		try {
			proc.terminate();
		} catch (klay::Exception& e) {
		}

		try {
			proc.waitForFinished();
		} catch (klay::Exception& e) {
		}
	} catch (klay::Exception& e) {
	}
}
