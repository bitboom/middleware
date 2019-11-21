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

#include <vist/logger/dlog.hpp>

#include <dlog.h>

#include <memory>

namespace vist {

/// Make logger backend as Dlog.
std::shared_ptr<LogBackend> Dlog::backend = LogStream::Init(std::make_shared<Dlog>());

void Dlog::info(const LogRecord& record) const noexcept
{
	SLOG(LOG_INFO, record.tag.c_str(), "%s", record.message.c_str());
}

void Dlog::debug(const LogRecord& record) const noexcept
{
	SLOG(LOG_DEBUG, record.tag.c_str(), "%s", record.message.c_str());
}

void Dlog::warn(const LogRecord& record) const noexcept
{
	SLOG(LOG_WARN, record.tag.c_str(), "%s", record.message.c_str());
}

void Dlog::error(const LogRecord& record) const noexcept
{
	SLOG(LOG_ERROR, record.tag.c_str(), "%s", record.message.c_str());
}

} // namespace vist
