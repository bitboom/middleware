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

namespace klay {

enum class KLAY_EXPORT LogLevel : int {
	Silent,
	Error,
	Warning,
	Debug,
	Info,
	Trace
};

struct KLAY_EXPORT LogRecord {
	LogLevel severity;
	std::string file;
	unsigned int line;
	std::string func;
	std::string message;
};

class KLAY_EXPORT Logger {
public:
	static void log(LogSink* logSink, const LogRecord record);
};

KLAY_EXPORT std::string LogLevelToString(const LogLevel level);
KLAY_EXPORT LogLevel StringToLogLevel(const std::string& level);

#ifndef __FILENAME__
#define __FILENAME__                                                  \
(::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define KSINK nullptr

#define FORMAT(items)                                                  \
(static_cast<std::ostringstream &>(std::ostringstream() << items)).str()

#define LOG(logsink, message, level)                                   \
do {                                                                   \
	audit::LogRecord record = { audit::LogLevel::level,                \
								__FILENAME__, __LINE__, __func__,      \
								FORMAT(message) };                     \
	audit::Logger::log(logsink, record);                               \
} while (0)

#define ERROR2(logsink, message) LOG(logsink, message, Error)
#define ERROR1(message) LOG(nullptr, message, Error)
#define WARN2(logsink, message) LOG(logsink, message, Warning)
#define WARN1(message) LOG(nullptr, message, Warning)

#if !defined(NDEBUG)
#define INFO2(logsink, message) LOG(logsink, message, Info)
#define INFO1(message) LOG(nullptr, message, Info)
#define DEBUG2(logsink, message) LOG(logsink, message, Debug)
#define DEBUG1(message) LOG(nullptr, message, Debug)
#define TRACE2(logsink, message) LOG(logsink, message, Trace)
#define TRACE1(message) LOG(nullptr, message, Trace)
#else
#define INFO2(logsink, message) do {} while (0)
#define INFO1(message) do {} while (0)
#define DEBUG2(logsink, message) do {} while (0)
#define DEBUG1(message) do {} while (0)
#define TRACE2(logsink, message) do {} while (0)
#define TRACE1(message) do {} while (0)
#endif //NDEBUG

#define GET_MACRO(_1, _2, macro, ...) macro
#define ERROR(args...) GET_MACRO(args, ERROR2, ERROR1)(args)
#define WARN(args...)  GET_MACRO(args, WARN2, WARN1)(args)
#define DEBUG(args...) GET_MACRO(args, DEBUG2, DEBUG1)(args)
#define INFO(args...)  GET_MACRO(args, INFO2, INFO1)(args)
#define TRACE(args...) GET_MACRO(args, TRACE2, TRACE1)(args)

} // namespace klay

namespace audit = klay;

#endif //__AUDIT_LOGGER_H__
