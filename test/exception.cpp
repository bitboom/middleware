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

#include <klay/exception.h>

#include <klay/testbench.h>

#include <string>

TESTCASE(Exception)
{
	std::string message("Exception Message");

	try {
		throw klay::Exception(message);
	} catch (const klay::Exception& e) {
		TEST_EXPECT(message, e.what());
	}
}

TESTCASE(AssertionViolationException)
{
	std::string message("Exception Message");

	try {
		throw klay::AssertionViolationException(message);
	} catch (const klay::AssertionViolationException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Assertion violation"), e.name());
		TEST_EXPECT(std::string("AssertionViolationException"), e.className());
	}
}

TESTCASE(NullPointerException)
{
	std::string message("Exception Message");

	try {
		throw klay::NullPointerException(message);
	} catch (const klay::NullPointerException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Null pointer"), e.name());
		TEST_EXPECT(std::string("NullPointerException"), e.className());
	}
}

TESTCASE(InvalidArgumentException)
{
	std::string message("Exception Message");

	try {
		throw klay::InvalidArgumentException(message);
	} catch (const klay::InvalidArgumentException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Invalid argument"), e.name());
		TEST_EXPECT(std::string("InvalidArgumentException"), e.className());
	}
}

TESTCASE(NotImplementedException)
{
	std::string message("Exception Message");

	try {
		throw klay::NotImplementedException(message);
	} catch (const klay::NotImplementedException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Not implemented"), e.name());
		TEST_EXPECT(std::string("NotImplementedException"), e.className());
	}
}

TESTCASE(RangeException)
{
	std::string message("Exception Message");

	try {
		throw klay::RangeException(message);
	} catch (const klay::RangeException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Out of range"), e.name());
		TEST_EXPECT(std::string("RangeException"), e.className());
	}
}

TESTCASE(NotFoundException)
{
	std::string message("Exception Message");

	try {
		throw klay::NotFoundException(message);
	} catch (const klay::NotFoundException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Not found"), e.name());
		TEST_EXPECT(std::string("NotFoundException"), e.className());
	}
}

TESTCASE(UnsupportedException)
{
	std::string message("Exception Message");

	try {
		throw klay::UnsupportedException(message);
	} catch (const klay::UnsupportedException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Unsupported"), e.name());
		TEST_EXPECT(std::string("UnsupportedException"), e.className());
	}
}

TESTCASE(TimeoutException)
{
	std::string message("Exception Message");

	try {
		throw klay::TimeoutException(message);
	} catch (const klay::TimeoutException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Timeout"), e.name());
		TEST_EXPECT(std::string("TimeoutException"), e.className());
	}
}

TESTCASE(NoPermissionException)
{
	std::string message("Exception Message");

	try {
		throw klay::NoPermissionException(message);
	} catch (const klay::NoPermissionException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("No permission"), e.name());
		TEST_EXPECT(std::string("NoPermissionException"), e.className());
	}
}

TESTCASE(OutOfMemoryException)
{
	std::string message("Exception Message");

	try {
		throw klay::OutOfMemoryException(message);
	} catch (const klay::OutOfMemoryException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("Out of memory"), e.name());
		TEST_EXPECT(std::string("OutOfMemoryException"), e.className());
	}
}

TESTCASE(IOException)
{
	std::string message("Exception Message");

	try {
		throw klay::IOException(message);
	} catch (const klay::IOException& e) {
		TEST_EXPECT(message, e.what());
		TEST_EXPECT(std::string("I/O error"), e.name());
		TEST_EXPECT(std::string("IOException"), e.className());
	}
}

TESTCASE(FallThrough)
{
	std::string message("Exception Message");

	try {
		throw klay::IOException(message);
	} catch (const klay::RangeException&) {
		TEST_FAIL("Exception mismatched.");
	} catch (const klay::NotFoundException&) {
		TEST_FAIL("Exception mismatched.");
	} catch (const klay::IOException& e) {
		TEST_EXPECT(message, e.what());
	}
}
