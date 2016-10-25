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
std::unique_ptr<std::string> Logger::tag([]() {
	auto tag = Logger::getTag();
	return tag.empty() ? new std::string("KLAY") : new std::string(tag);
}());

std::unique_ptr<LogSink> Logger::backend([]() {
	auto *backend = Logger::getBackend();
	return backend != nullptr ? std::move(backend) : new ConsoleLogSink();
}());

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

LogLevel StringToLogLevel(const std::string& level)
{
	if (level == "ERROR")
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

void Logger::setLogLevel(const LogLevel level)
{
	Logger::logLevel = level;
}

LogLevel Logger::getLogLevel(void)
{
	return Logger::logLevel;
}

void Logger::setTag(const std::string& tag)
{
	Logger::tag.reset(new std::string(tag));
}

std::string Logger::getTag(void)
{
	auto *pTag = Logger::tag.get();
	return (pTag != nullptr) ? *pTag : std::string();
}

void Logger::setBackend(LogSink *logSink)
{
	if (logSink == nullptr)
		return;

	Logger::backend.reset(logSink);
}

LogSink *Logger::getBackend(void)
{
	auto *pBackend = Logger::backend.get();
	return (pBackend != nullptr) ? pBackend : nullptr;
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
		   << ", " << func << "() > " << message
		   << std::endl;

	Logger::backend->sink(buffer.str());
}

} // namespace audit
