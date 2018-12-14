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

#include <unistd.h>
#include <sys/types.h>

#include <stdexcept>

#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/audit/logger-core.h>
#include <klay/audit/console-sink.h>

namespace klay {

std::string LogLevelToString(const LogLevel level)
{
	switch (level) {
	case LogLevel::Silent:
		return "SILENT";
	case LogLevel::Error:
		return "ERROR";
	case LogLevel::Warning:
		return "WARN";
	case LogLevel::Debug:
		return "DEBUG";
	case LogLevel::Info:
		return "INFO";
	default:
		return "UNKNOWN";
	}
}

LogLevel StringToLogLevel(const std::string& level)
{
	if (level == "SILENT")
		return LogLevel::Silent;
	else if (level == "ERROR")
		return LogLevel::Error;
	else if (level == "WARN")
		return LogLevel::Warning;
	else if (level == "DEBUG")
		return LogLevel::Debug;
	else if (level == "INFO")
		return LogLevel::Info;
	else
		return LogLevel::Trace;
}

void Logger::log(LogSink* logSink, const LogRecord record)
{
	std::ostringstream buffer;

	buffer << LogLevelToString(record.severity)
		   << "<" << ::getpid() << ">:"
		   << record.file << ":" << record.line
		   << ", " << record.func << "() > " << record.message
		   << std::endl;

	LoggerCore::GetInstance().dispatch(logSink, buffer.str());
}

} // namespace klay
