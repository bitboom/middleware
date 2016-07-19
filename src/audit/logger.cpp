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
#include <klay/audit/console-sink.h>

namespace audit {

LogLevel Logger::logLevel = LogLevel::Trace;
std::unique_ptr<LogSink> Logger::backend(new ConsoleLogSink());

std::string LogLevelToString(const LogLevel level)
{
	switch (level) {
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

void Logger::setLogLevel(const LogLevel level)
{
	Logger::logLevel = level;
}

LogLevel Logger::getLogLevel(void)
{
	return Logger::logLevel;
}

void Logger::log(LogLevel severity,
				 const std::string& file,
				 const unsigned int line,
				 const std::string& func,
				 const std::string& message)
{
	std::ostringstream buffer;

	buffer << LogLevelToString(severity)
		   << "<" << ::getpid() << ">:"
		   << file << ":" << line
		   << " " << func << " " << message
		   << std::endl;

	Logger::backend->sink(buffer.str());
}

} // namespace audit
