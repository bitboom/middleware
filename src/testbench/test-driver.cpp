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

#include <klay/testbench/test-driver.h>

namespace klay {
namespace testbench {

std::unique_ptr<TestDriver> TestDriver::instance = nullptr;
std::once_flag TestDriver::flag;

TestDriver& TestDriver::GetInstance()
{
	std::call_once(TestDriver::flag, []() {
		TestDriver::instance.reset(new TestDriver);
	});

	return *TestDriver::instance;
}

void TestDriver::addTestCase(TestCase* testCase)
{
	this->testSuite.addTestCase(testCase);
}

void TestDriver::addFailure(const std::string& name, const Source& source)
{
	this->reporter.addFailure(name, source);
}

void TestDriver::run(void)
{
	const auto& testCases = this->testSuite.getTestCases();
	for (const auto& tc : testCases) {
		auto startTime = this->reporter.start(tc->getName());

		try {
			tc->task();
		} catch (...) {
			this->reporter.addException(tc->getName());
		}

		this->reporter.end(tc->getName(), startTime);
	}

	this->reporter.report();
}

} //namespace testbench
} //namespace klay
