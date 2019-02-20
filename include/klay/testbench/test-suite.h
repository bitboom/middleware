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

#pragma once

#include <klay/klay.h>
#include <klay/testbench/test-case.h>

#include <memory>
#include <vector>

namespace klay {
namespace testbench {

class KLAY_EXPORT TestSuite {
public:
	TestSuite() = default;
	TestSuite(const std::string& name);
	virtual ~TestSuite() = default;

	void addTestCase(TestCase* testCase) noexcept;

	const std::vector<TestCase*>& getTestCases(void) const noexcept;

	const std::string& getName(void) const noexcept;

private:
	std::vector<TestCase*> testCases;

	std::string name;
};

} // namespace testbench
} // namespace klay
