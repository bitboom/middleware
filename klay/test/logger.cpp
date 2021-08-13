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

#include <klay/error.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/audit/console-sink.h>
#include <klay/audit/dlog-sink.h>
#include <klay/audit/null-sink.h>

#include <klay/testbench.h>

TESTCASE(LogMacroTest)
{
	TRACE("Trace");
	INFO("Info");
	DEBUG("Debug");
	WARN("Warning");
	ERROR("Error");
}

TESTCASE(LogSeverityTest)
{
	try {
		audit::LogLevelToString((audit::LogLevel)-1);
	} catch (klay::Exception& e) {
	}
}

TESTCASE(DefaultLogTest)
{
	TRACE(KSINK, "Trace");
	INFO(KSINK, "Info");
	DEBUG(KSINK, "Debug");
	WARN(KSINK, "Warn");
	ERROR(KSINK, "Error");
}

TESTCASE(LogSinkTest)
{
	audit::DlogLogSink* dlog = new audit::DlogLogSink("KLAY");
	INFO(dlog, "Dlog Test : " << "Info");
	DEBUG(dlog, "Dlog Test : " << "Debug");
	WARN(dlog, "Dlog Test : " << "Warning");
	ERROR(dlog, "Dlog Test : " << "Error");
	delete dlog;

	audit::ConsoleLogSink* console = new audit::ConsoleLogSink();
	INFO(console, "Console Test : " << "Info");
	DEBUG(console, "Console Test : " << "Debug");
	WARN(console, "Console Test : " << "Warning");
	ERROR(console, "Console Test : " << "Error");
	delete console;

	audit::NullLogSink* null = new audit::NullLogSink();
	INFO(null, "Null Test : " << "Info");
	DEBUG(null, "Null Test : " << "Debug");
	WARN(null, "Null Test : " << "Warning");
	ERROR(null, "Null Test : " << "Error");
	delete null;
}

TESTCASE(MultiLogSinkTest)
{
	audit::DlogLogSink* dlog = new audit::DlogLogSink("KLAY");
	audit::DlogLogSink* dlog2 = new audit::DlogLogSink("KLAY2");

	INFO(dlog, "Dlog Test : " << "Info");
	INFO(dlog2, "Dlog Test : " << "Info");
	DEBUG(dlog, "Dlog Test : " << "Debug");
	DEBUG(dlog2, "Dlog Test : " << "Debug");
	WARN(dlog, "Dlog Test : " << "Warning");
	WARN(dlog2, "Dlog Test : " << "Warning");
	ERROR(dlog, "Dlog Test : " << "Error");
	ERROR(dlog2, "Dlog Test : " << "Error");

	delete dlog;
	delete dlog2;
}
