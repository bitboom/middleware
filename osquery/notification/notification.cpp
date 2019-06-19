/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <mutex>

#include <osquery/notification.h>
#include <osquery/logger.h>

namespace {
	std::mutex mutex;
} // anonymous namespace

namespace osquery {

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

} // namespace osquery
