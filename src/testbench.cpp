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

#include <string.h>
#include <iostream>

#include <klay/testbench.h>

namespace testbench {

Source::Source(const std::string& file, long line, const std::string& msg) :
	fileName(file), lineNumber(line), message(msg)
{
}

TestResult::TestResult() :
	__failureCount(0)
{
}

TestResult::~TestResult()
{
}

void TestResult::testsStarted()
{
}

void TestResult::addFailure(const std::string& name, const Source& source)
{
	std::cout << "Testcase \"" << name << "\""
			  << " failed: \"" << source.message << "\""
			  << " line " << source.lineNumber
			  << " in " << source.fileName << std::endl;

	__failureCount++;
}

void TestResult::testsEnded()
{
	if (__failureCount > 0) {
		std::cout << "\nThere were " << __failureCount << " failures" << std::endl;
	} else {
		std::cout << "\nThere were no test failures" << std::endl;
	}
}

TestSuite::TestSuite(const std::string& name)
	: __testName(name)
{
	Testbench::addTestSuite(this);
}

TestSuite::~TestSuite()
{
}

void TestSuite::setup()
{
}

void TestSuite::teardown()
{
}

void TestSuite::run()
{
	setup();

	TestCaseRegistry::iterator iter = __registry.begin();
	while (iter != __registry.end()) {
		TestSuite::TestCase& testcase = (*iter);

		std::cout << "Entering testcase: "
				  << testcase.testName << std::endl;
		try {
			(this->*testcase.function)();
		} catch (...) {
			TEST_FAIL("Caught exception from " +
					  testcase.testName + " testcase");
		}
		std::cout << "Leaving testcase: "
				  << testcase.testName << std::endl;

		iter++;
	}

	teardown();
}

void TestSuite::registerTestCase(TestFunction func, const std::string& name)
{
	__registry.push_back(TestCase(func, name));
}

bool TestSuite::check(long expected, long actual, const std::string& file, long line)
{
	if (expected == actual) {
		return true;
	}

	std::stringstream stream;
	stream << "expected " << expected << " but it was " << actual;
	Testbench::report(__testName, Source(file, line, stream.str()));

	return false;
}

std::unique_ptr<TestResult> Testbench::collector(new TestResult());

void Testbench::addTestSuite(TestSuite *testSuite)
{
	instance().add(testSuite);
}

void Testbench::runAllTestSuites()
{
	instance().run();
}

Testbench& Testbench::instance()
{
	static Testbench testbench;
	return testbench;
}

void Testbench::add(TestSuite *testSuite)
{
	__testSuites.push_back(testSuite);
}

void Testbench::report(const std::string& name, const Source& source)
{
	collector->addFailure(name, source);
}

void Testbench::run()
{
	collector->testsStarted();

	TestSuiteRegistry::iterator iter = __testSuites.begin();
	while (iter != __testSuites.end()) {
		try {
			(*iter)->run();
		} catch (...) {
			// Logging exception
		}
		iter++;
	}

	collector->testsEnded();
}

} //namespace testbench
