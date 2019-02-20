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

TestSuite::TestSuite(const std::string& name) : name(name)
{
}

void TestSuite::addTestCase(TestCase* testCase) noexcept
{
	this->testCases.push_back(testCase);
}

const std::vector<TestCase*>& TestSuite::getTestCases(void) const noexcept
{
	return this->testCases;
}

const std::string& TestSuite::getName(void) const noexcept
{
	return this->name;
}

} //namespace testbench
} //namespace klay
