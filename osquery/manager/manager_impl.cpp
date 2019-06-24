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

#include <osquery/core.h>
#include <osquery/filesystem.h>
#include <osquery/flags.h>
#include <osquery/logger.h>
#include <osquery/notification.h>
#include <osquery/sql.h>
#include <osquery/status.h>

#include <gflags/gflags.h>

#include <boost/filesystem/operations.hpp>

#include <sstream>

namespace osquery {

ManagerImpl::ManagerImpl()
{
	auto logDir = Flag::get().getValue("osquery_log_dir");
	if (!logDir.empty() && !(pathExists(logDir).ok()))
		boost::filesystem::create_directories(logDir);

	// TODO(Sangwan): Get debug mode at build time
	Flag::updateValue("verbose_debug", "true");

	const char* cargv[] = {"tizen-osquery"};
	osquery::initOsquery(1, const_cast<char**>(cargv));

	LOG(INFO) << "Initalize osquery manager. ";
}

ManagerImpl::~ManagerImpl() noexcept
{
	LOG(INFO) << "Shutdown osquery manager.";
	osquery::shutdownOsquery();
}

ManagerImpl& ManagerImpl::instance()
{
	static ManagerImpl instance;
	return instance;
}

Rows ManagerImpl::execute(const std::string& query)
{
	LOG(INFO) << "Execute query: " << query;

	osquery::QueryData results;
	auto status = osquery::query(query, results);
	if (!status.ok())
		LOG(ERROR) << "Executing query failed: " << status.getCode();

	return results;
}

void ManagerImpl::subscribe(const std::string& table, const Callback& callback)
{
	LOG(INFO) << "Subscribe event: " << table;

	auto status = Notification::instance().add(table, callback);
	if (!status.ok())
		LOG(ERROR) << "Subscribing event failed: " << status.getCode();
}

std::vector<std::string> ManagerImpl::tables(void) noexcept
{
	return SQL::getTableNames();
}

std::vector<std::string> ManagerImpl::columns(const std::string& table) noexcept
{
	std::stringstream query;
	query << "SELECT * FROM " << table;

	tables::TableColumns columns;
	getQueryColumns(query.str(), columns);

	// Extract column names
	std::vector<std::string> names;
	for (auto& c : columns)
		names.emplace_back(std::move(c.first));

	return names;
}

} // namespace osquery
