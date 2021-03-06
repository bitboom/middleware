/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <iostream>

#include <dlog.h>

#include <klay/audit/dlog-sink.h>
#include <klay/audit/logger.h>

namespace klay {

DlogLogSink::DlogLogSink(const std::string& tag) : tag(tag) {}

void DlogLogSink::sink(const std::string &message)
{
	auto level = message.substr(0, message.find('<'));
	auto subMsg = message.substr(message.find(':') + 1);

	switch (StringToLogLevel(level)) {
	case LogLevel::Error:
		SLOG(LOG_ERROR, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	case LogLevel::Warning:
		SLOG(LOG_WARN, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	case LogLevel::Debug:
		SLOG(LOG_DEBUG, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	case LogLevel::Info:
		SLOG(LOG_INFO, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	case LogLevel::Trace:
		SLOG(LOG_VERBOSE, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	default:
		SLOG(LOG_SILENT, this->tag.c_str(), "%s", subMsg.c_str());
		return;
	}
}

} // namespace klay
