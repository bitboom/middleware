/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __KLAY_TESTBENCH_H__
#define __KLAY_TESTBENCH_H__

#include <klay/klay.h>
#include <klay/testbench/test-driver.h>
#include <klay/testbench/test-reporter.h>

#include <cstring>
#include <string>
#include <sstream>

namespace klay {
namespace testbench {

class KLAY_EXPORT Testbench {
public:
	static void runAllTestSuites();
};

struct KLAY_EXPORT Source;

#ifndef __FILENAME__
#define __FILENAME__                                                 \
(::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define TESTCASE(TestName)                                             \
class TestName##TestCase final : public klay::testbench::TestCase {    \
public:                                                                \
	TestName##TestCase() : TestCase(#TestName)                         \
	{                                                                  \
		klay::testbench::TestDriver::GetInstance().addTestCase(this);  \
	}                                                                  \
	void task(void) override;                                          \
} TestName##TestCase##Instance;                                        \
void TestName##TestCase::task()

#define TEST_EXPECT(expected, actual)                                             \
{                                                                                 \
	__typeof__(expected) exp = (expected);                                        \
	__typeof__(actual) act = (actual);                                            \
	if (exp != act) {                                                             \
		std::stringstream ss;                                                     \
		ss << "expected " << exp << " but it was " << act;                        \
		klay::testbench::TestDriver::GetInstance().addFailure(this->getName(),    \
		klay::testbench::Source(__FILENAME__, __LINE__, ss.str()));               \
	}                                                                             \
}

#define TEST_FAIL(text)                                                       \
{                                                                             \
	klay::testbench::TestDriver::GetInstance().addFailure(this->getName(),    \
	klay::testbench::Source(__FILENAME__, __LINE__, (text)));                 \
	return;                                                                   \
}

} // namespace testbench
} // namespace klay

namespace testbench = klay::testbench;

#endif //!__KLAY_TESTBENCH_H__
