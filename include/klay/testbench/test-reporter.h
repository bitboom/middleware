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
#include <klay/colorize.h>
#include <klay/testbench/test-case.h>

#include <chrono>
#include <string>
#include <iostream>

namespace klay {
namespace testbench {

using namespace std::chrono;
using TimePoint = time_point<system_clock>;

struct KLAY_EXPORT Source {
	Source(const std::string& file, long line, const std::string& msg);

	std::string fileName;
	long lineNumber;
	std::string message;
};

class KLAY_EXPORT TestReporter final {
public:
	TimePoint start(const std::string& name) noexcept;
	void end(const std::string& name, TimePoint startPoint) noexcept;

	void addFailure(const std::string& name, const Source& source) noexcept;
	void addException(const std::string& name) noexcept;
	void report(void) const noexcept;

private:
	std::size_t total = 0;
	std::size_t failed = 0;
};

} // namespace testbench
} // namespace klay
