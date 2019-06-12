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
 * @file osquery-manager.h
 * @brief Osquery manager C++ API header
 */


#pragma once


#include <string>
#include <memory>
#include <map>
#include <vector>

namespace osquery {

using Row = std::map<std::string, std::string>;
using Rows = std::vector<Row>;

class OsqueryManager final {
public:
	OsqueryManager(const OsqueryManager&) = delete;
	OsqueryManager& operator=(const OsqueryManager&) = delete;

	/// TBD: Consider error handling.
	static Rows execute(const std::string& query);
	static std::vector<std::string> tables(void) noexcept;
	static std::vector<std::string> columns(const std::string& table) noexcept;

private:
	OsqueryManager();
	~OsqueryManager() noexcept;

	static OsqueryManager& instance();

	/// TODO(Sangwan): Apply pimpl idiom.
	Rows executeInternal(const std::string& query) const;
	std::vector<std::string> tablesInternal(void) const noexcept;
	std::vector<std::string> columnsInternal(const std::string& table) const noexcept;
};

} // namespace osquery
