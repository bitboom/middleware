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
	} catch (runtime::Exception& e) {
	}
}

TESTCASE(BackendTest)
{
	audit::Logger::setBackend(new audit::DlogLogSink());
	audit::Logger::setTag("KLAY");
	INFO("Dlog Test : " << "Info");
	DEBUG("Dlog Test : " << "Debug");
	WARN("Dlog Test : " << "Warning");
	ERROR("Dlog Test : " << "Error");

	audit::Logger::setBackend(new audit::ConsoleLogSink());
	INFO("Console Test : " << "Info");
	DEBUG("Console Test : " << "Debug");
	WARN("Console Test : " << "Warning");
	ERROR("Console Test : " << "Error");

	audit::Logger::setBackend(new audit::NullLogSink());
	INFO("Null Test : " << "Info");
	DEBUG("Null Test : " << "Debug");
	WARN("Null Test : " << "Warning");
	ERROR("Null Test : " << "Error");
}
