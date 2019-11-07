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

#include "notification.hpp"

#include <mutex>

#include <osquery/logger.h>

namespace {
	std::mutex mutex;
} // anonymous namespace

namespace vist {

using namespace osquery;

Notification& Notification::instance()
{
	static Notification notifier;
	return notifier;
}

Status Notification::add(const std::string& table, const NotifyCallback& callback)
{
	if (table.empty())
		return Status(1, "Wrong table name");

	LOG(INFO) << "Add NotifyCallback to:" << table;
	{
		std::lock_guard<std::mutex> lock(mutex);
		this->callbacks.insert(std::make_pair(table, callback));
	}

	return Status(0, "OK");
}

Status Notification::emit(const std::string& table, const Row& result) const
{
	if (table.empty())
		return Status(1, "Wrong table name");

	auto iter = this->callbacks.find(table);
	if (iter == this->callbacks.end())
		return Status(1, "Registered callback not found");

	LOG(INFO) << "Emit notification about:" << table;
	{
		std::lock_guard<std::mutex> lock(mutex);
		while (iter != this->callbacks.end())  {
			const auto& callback = iter->second;
			callback(result);
			iter++;
		}
	}

	return Status(0, "OK");
}

} // namespace vist
