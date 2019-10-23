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


/**
 * @file notification.h
 * @brief Notify to registered stuffs when event-callback called
 */


#pragma once

#include <map>
#include <vector>

#include <osquery/status.h>

namespace vist {

using Row = std::map<std::string, std::string>;
using Rows = std::vector<Row>;

using Callback = std::function<void(const Row&)>;

using NotifyCallback = Callback;

class Notification final {
public:
	static Notification& instance();

	Notification(const Notification&) = delete;
	Notification& operator=(const Notification&) = delete;

	osquery::Status add(const std::string& table, const NotifyCallback& callback);
	osquery::Status emit(const std::string& table, const Row& result) const;

private:
	Notification() = default;
	~Notification() = default;

	std::multimap<std::string, NotifyCallback> callbacks;
};

} // namespace vist
