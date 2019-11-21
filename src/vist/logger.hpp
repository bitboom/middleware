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
 * @brief Header Only Library for logging.
 * @usage
 *  boilerplate : ${LEVEL}(${TAG}) << ${MESSAGE_STREAM}
 *
 *  INFO("vist") << "Info message" << 1;
 *  DEBUG("vist") << "Debug message" << 2 << 'a';
 *  WARN(VIST) << "Warning message" << 3 << 'b' << true;
 *  ERROR(VIST) << "Error message" << 4 << 'c' << false << 0.0f;
 */

#pragma once

#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <iostream>

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

struct LogBackend {
	LogBackend() = default;
	virtual ~LogBackend() = default;

	LogBackend(const LogBackend&) = delete;
	LogBackend& operator=(const LogBackend&) = delete;

	LogBackend(LogBackend&&) = default;
	LogBackend& operator=(LogBackend&&) = default;

	virtual void info(const LogRecord& record) const noexcept = 0;
	virtual void debug(const LogRecord& record) const noexcept = 0;
	virtual void warn(const LogRecord& record) const noexcept = 0;
	virtual void error(const LogRecord& record) const noexcept = 0;
};

struct Console final : public LogBackend {
	enum class ColorCode {
		Black = 30,
		Red = 31,
		Green = 32,
		Yellow = 33,
		Blue = 34,
		Magenta = 35,
		Cyan = 36,
		White = 37,
		Default = 39
	};

	struct Colorize {
		explicit Colorize(ColorCode code = ColorCode::Default) : code(code) {}

		friend std::ostream& operator<<(std::ostream& os, const Colorize& c)
		{
			return os << "\033[" << static_cast<int>(c.code) << "m";
		}

		ColorCode code;
	};

	void info(const LogRecord& record) const noexcept override
	{
		std::cout << Colorize(ColorCode::Green);
		std::cout << "[I][" << record.tag << "]" << record.message << std::endl;
		std::cout << Colorize(ColorCode::Default);
	}

	void debug(const LogRecord& record) const noexcept override
	{
		std::cout << Colorize(ColorCode::Default);
		std::cout << "[D][" << record.tag << "]" << record.message << std::endl;
	}

	void warn(const LogRecord& record) const noexcept override
	{
		std::cout << Colorize(ColorCode::Magenta);
		std::cout << "[W][" << record.tag << "]" << record.message << std::endl;
		std::cout << Colorize(ColorCode::Default);
	}

	void error(const LogRecord& record) const noexcept override
	{
		std::cout << Colorize(ColorCode::Red);
		std::cout << "[E][" << record.tag << "]" << record.message << std::endl;
		std::cout << Colorize(ColorCode::Default);
	}
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

	static std::shared_ptr<LogBackend> Init(std::shared_ptr<LogBackend>&& backend = nullptr)
	{
		static std::shared_ptr<LogBackend> base = std::make_shared<Console>();
		if (backend != nullptr)
			base = backend;

		return base;
	}

private:
	inline void log() const noexcept
	{
		auto backend = Init();
		switch (record.level) {
		case LogLevel::Info:
			backend->info(this->record);
			return;
		case LogLevel::Debug:
			backend->debug(this->record);
			return;
		case LogLevel::Warn:
			backend->warn(this->record);
			return;
		case LogLevel::Error:
			backend->error(this->record);
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
