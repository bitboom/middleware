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
#include <klay/testbench/test-suite.h>
#include <klay/testbench/test-reporter.h>

#include <string>
#include <memory>
#include <mutex>

namespace klay {
namespace testbench {

class KLAY_EXPORT TestDriver final {
public:
	~TestDriver() = default;

	TestDriver(const TestDriver &) = delete;
	TestDriver(TestDriver &&) = delete;
	TestDriver &operator=(const TestDriver &) = delete;
	TestDriver &operator=(TestDriver &&) = delete;

	static TestDriver& GetInstance(void);

	void addTestCase(TestCase* testCase);
	void addFailure(const std::string& name, const Source& source);

	void run(void);

private:
	TestDriver() = default;

	// TODO(sangwan.kwon): Support multiple TestSuite
	TestSuite testSuite;
	TestReporter reporter;

	static std::unique_ptr<TestDriver> instance;
	static std::once_flag flag;
};

} // namespace testbench
} // namespace klay
