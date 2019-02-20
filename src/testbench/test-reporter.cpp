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

#include <klay/testbench/test-reporter.h>

namespace klay {
namespace testbench {

Source::Source(const std::string& file, long line, const std::string& msg)
	: fileName(file), lineNumber(line), message(msg)
{
}

TimePoint TestReporter::start(const std::string& name) noexcept
{
	std::cout << "Entering testcase: " << name << std::endl;

	this->total++;

	return system_clock::now();
}

void TestReporter::end(const std::string& name, TimePoint startPoint) noexcept
{
	auto endPoint = system_clock::now();
	auto ms = duration_cast<milliseconds>(endPoint - startPoint);
	auto time = ms.count();

	std::cout << "Leaving testcase: " <<  name
			  << " (Elapsed time: " << time
			  << "ms)" << std::endl;
}

void TestReporter::addFailure(const std::string& name, const Source& source) noexcept
{
	std::cout << Colorize(RED)
			  << "\tTestcase \"" << name << "\""
			  << " failed: \"" << source.message << "\""
			  << " line " << source.lineNumber
			  << " in " << source.fileName << std::endl
			  << Colorize(DEFAULT);

	this->failed++;
}

void TestReporter::addException(const std::string& name) noexcept
{
	std::cout << Colorize(RED)
			  << "\tTestcase \"" << name << "\""
			  << " exception occured."
			  << Colorize(DEFAULT);

	this->failed++;
}

void TestReporter::report(void) const noexcept
{
	if (this->failed != 0)
		std::cout << Colorize(RED)
				  << "\nThere were " << this->failed << " failures in total: "
				  << this->total << std::endl
				  << Colorize(DEFAULT);
	else
		std::cout << Colorize(GREEN)
				  << "\nThere were no test failures in total: "
				  << this->total << std::endl
				  << Colorize(DEFAULT);
}

} //namespace testbench
} //namespace klay
