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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or ManagerImplied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file manager_impl.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation of osquery manager's impl
 */

#include "manager_impl.h"

#include <osquery/registry.h>
#include <osquery/core.h>
#include <osquery/filesystem/filesystem.h>
#include <osquery/flags.h>
#include <osquery/logger.h>
#include <osquery/notification.h>
#include <osquery/sql.h>
#include <osquery/status.h>

#include <gflags/gflags.h>

#include <boost/filesystem/operations.hpp>

#include <functional>
#include <sstream>
#include <tuple>

namespace osquery {

ManagerImpl::ManagerImpl()
{
	auto logDir = Flag::getValue("osquery_log_dir");
	if (!logDir.empty() && !(pathExists(logDir).ok()))
		boost::filesystem::create_directories(logDir);

	LOG(INFO) << "Initalize osquery manager. ";

	registryAndPluginInit();
}

ManagerImpl::~ManagerImpl() noexcept
{
	LOG(INFO) << "Shutdown osquery manager.";
}

ManagerImpl& ManagerImpl::instance()
{
	static ManagerImpl instance;
	return instance;
}

Rows ManagerImpl::execute(const std::string& query)
{
	LOG(INFO) << "Execute query: " << query;
	osquery::SQL sql(query, true);
	if (!sql.ok()) {
		LOG(ERROR) << "Executing query failed: " << sql.getMessageString();
		return Rows();
	}

	return std::move(sql.rows());
}

void ManagerImpl::subscribe(const std::string& table, const Callback& callback)
{
	LOG(INFO) << "Subscribe event: " << table;

	auto status = Notification::instance().add(table, callback);
	if (!status.ok())
		LOG(ERROR) << "Subscribing event failed: " << status.getCode();
}

std::vector<std::string> ManagerImpl::columns(const std::string& table) noexcept
{
	std::stringstream query;
	query << "SELECT * FROM " << table;

	TableColumns columns;
	getQueryColumns(query.str(), columns);

	// Extract column names
	std::vector<std::string> names;
	for (auto& c : columns)
		names.emplace_back(std::move(std::get<0>(c)));

	return names;
}

} // namespace osquery
