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

#include <vist/logger/glog.hpp>

#undef ERROR
#include <glog/logging.h>

namespace vist {

Glog::Glog()
{
	/// osquery calls below function
	// google::InitGoogleLogging("ViST");

	FLAGS_logtostderr = 1;
}

Glog::~Glog()
{
	// google::ShutdownGoogleLogging();
}

void Glog::info(const LogRecord& record) const noexcept
{
	LOG(INFO) << "[" << record.tag << "]" << record.message << std::endl;
}

void Glog::debug(const LogRecord& record) const noexcept
{
	DLOG(INFO) << "[" << record.tag << "]" << record.message << std::endl;
}

void Glog::warn(const LogRecord& record) const noexcept
{
	LOG(WARNING) << "[" << record.tag << "]" << record.message << std::endl;
}

void Glog::error(const LogRecord& record) const noexcept
{
	LOG(ERROR) << "[" << record.tag << "]" << record.message << std::endl;
}

} // namespace vist
