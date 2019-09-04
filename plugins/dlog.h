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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or DLogied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#pragma once

#include <klay/audit/logger.h> 
#include <klay/audit/dlog-sink.h>

#define PLUGINS osquery::DLog::getSink()

namespace osquery {

class DLog final {
public:
	DLog(const DLog&) = delete;
	DLog& operator=(const DLog&) = delete;

	DLog(DLog&&) noexcept = default;
	DLog& operator=(DLog&&) noexcept = default;

	static inline DLog& instance()
	{
		static DLog dlog;
		return dlog;
	}

	static inline audit::LogSink* getSink()
	{
		return DLog::instance().logSink.get();
	}

private:
	DLog()
	{
		auto dlog = new audit::DlogLogSink("DPM_PLUGIN");
		this->logSink.reset(dynamic_cast<audit::LogSink*>(dlog));
	}
	~DLog() noexcept = default;

	std::unique_ptr<audit::LogSink> logSink;
};

} // namespace osquery
