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

#pragma once

#include <klay/audit/dlog-sink.h>
#include <klay/audit/logger.h>

#include <memory>

#define VIST vist::Logger::Instance()
#define VIST_CLIENT vist::Logger::ClientInstance()

namespace vist {

class Logger final {
public:
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	Logger(Logger&&) = default;
	Logger& operator=(Logger&&) = default;

	static klay::LogSink* Instance()
	{
		static Logger instance("VIST");
		return &instance.logSink;
	}

	static klay::LogSink* ClientInstance()
	{
		static Logger instance("VIST_CLIENT");
		return &instance.logSink;
	}

private:
	explicit Logger(const std::string& tag) : logSink(tag) {}
	~Logger() = default;

	klay::DlogLogSink logSink;
};

} // namespace vist
