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

#ifndef __AUDIT_LOGGER_H__
#define __AUDIT_LOGGER_H__

#include <cstring>
#include <string>
#include <memory>
#include <sstream>
#include <iostream>

#include "logsink.h"

namespace audit {

enum class LogLevel : int {
	Error,
	Warning,
	Debug,
	Info,
	Trace
};

class Logger {
public:
	static void setLogLevel(const LogLevel level);
	static LogLevel getLogLevel(void);
	static void setTag(const std::string& tag);
	static std::string getTag(void);
	static void setBackend(LogSink *logSink);
	static void log(LogLevel severity,
					const std::string& file,
					const unsigned int line,
					const std::string& func,
					const std::string& message);

private:
	static LogSink *getBackend(void);

	static LogLevel logLevel;
	static std::unique_ptr<std::string> tag;
	static std::unique_ptr<LogSink> backend;
};

#ifndef __FILENAME__
#define __FILENAME__                                                   \
(::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define FORMAT(ITEMS)                                                  \
(static_cast<std::ostringstream &>(std::ostringstream() << ITEMS)).str()

#define LOG(SEVERITY, MESSAGE)                                         \
do {                                                                   \
	if (audit::LogLevel::SEVERITY <= audit::Logger::getLogLevel()) {   \
		audit::Logger::log(audit::LogLevel::SEVERITY,                  \
						   __FILENAME__, __LINE__, __func__,           \
						   FORMAT(MESSAGE));                           \
	}                                                                  \
} while (0)

#define ERROR(MESSAGE)	LOG(Error, MESSAGE)
#define WARN(MESSAGE)	LOG(Warning, MESSAGE)

#if !defined(NDEBUG)
#define INFO(MESSAGE)	LOG(Info, MESSAGE)
#define DEBUG(MESSAGE)	LOG(Debug, MESSAGE)
#define TRACE(MESSAGE)	LOG(Trace, MESSAGE)
#else
#define INFO(MESSAGE)  do {} while (0)
#define DEBUG(MESSAGE) do {} while (0)
#define TRACE(MESSAGE) do {} while (0)
#endif //NDEBUG

std::string LogLevelToString(const LogLevel level);
LogLevel StringToLogLevel(const std::string& level);

} // namespace audit
#endif //__AUDIT_LOGGER_H__
