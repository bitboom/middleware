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
/*
 * @brief Logging macro for dlog.
 * @usage
 *  boilerplate : ${LEVEL}(${TAG}) << ${MESSAGE_STREAM}
 *
 *  INFO(VIST) << "Info message" << 1;
 *  DEBUG(VIST) << "Debug message" << 2 << 'a';
 *  WARN(VIST) << "Warning message" << 3 << 'b' << true;
 *  ERROR(VIST) << "Error message" << 4 << 'c' << false << 0.0f;
 */

#pragma once

#include <cstring>
#include <memory>
#include <sstream>
#include <string>

#include <dlog.h>

namespace vist {

enum class LogLevel {
	Info,
	Debug,
	Warn,
	Error
};

struct LogRecord {
	LogLevel level = LogLevel::Debug;
	std::string tag;
	std::string file;
	unsigned int line = 0;
	std::string func;
	std::string message;
};

class LogStream final {
public :
	LogStream(LogRecord record) noexcept : record(std::move(record)) {}
	~LogStream() noexcept
	{
		std::stringstream format;
		format << "[" << record.file << ":" << record.line
			   << " " << record.func << "()] ";

		record.message += (format.str() + stream.str());
		log();
	}

	LogStream(const LogStream&) = delete;
	LogStream& operator=(const LogStream&) = delete;

	LogStream(LogStream&&) = default;
	LogStream& operator=(LogStream&&) = default;

	template<typename T>
	LogStream& operator<<(const T& arg)
	{
		stream << arg;
		return *this;
	}

private:
	inline void log() const noexcept
	{
		switch (record.level) {
		case LogLevel::Info:
			SLOG(LOG_INFO, record.tag.c_str(), "%s", record.message.c_str());
			return;
		case LogLevel::Debug:
			SLOG(LOG_DEBUG, record.tag.c_str(), "%s", record.message.c_str());
			return;
		case LogLevel::Warn:
			SLOG(LOG_WARN, record.tag.c_str(), "%s", record.message.c_str());
			return;
		case LogLevel::Error:
			SLOG(LOG_ERROR, record.tag.c_str(), "%s", record.message.c_str());
			return;
		}
	}

	LogRecord record;
	std::ostringstream stream;
};

#ifndef __FILENAME__
#define __FILENAME__ \
(::strrchr(__FILE__, '/') ? ::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOG(level, tag) vist::LogStream(LOGCAT(vist::LogLevel::level, #tag))
#define LOGCAT(level, tag) \
	vist::LogRecord {level, tag, __FILENAME__, __LINE__, __func__}

#define NULLSTREAM std::ostringstream()

#define INFO(tag)  LOG(Info, tag)
#define ERROR(tag) LOG(Error, tag)

#if !defined(NDEBUG)
#define DEBUG(tag) LOG(Debug, tag)
#define WARN(tag)  LOG(Warn, tag)
#else
#define DEBUG(tag) NULLSTREAM
#define WARN(tag) NULLSTREAM
#endif /// NDEBUG

} // namespace vist
