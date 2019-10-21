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
/*
 * @file manager.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation of osquery manager
 */

#include <osquery_manager.h>

#include "manager_impl.h"

namespace osquery {

Rows OsqueryManager::execute(const std::string& query)
{
	return ManagerImpl::instance().execute(query);
}

void OsqueryManager::subscribe(const std::string& table, const Callback& callback)
{
	return ManagerImpl::instance().subscribe(table, callback);
}

std::vector<std::string> OsqueryManager::columns(const std::string& table) noexcept
{
	return ManagerImpl::instance().columns(table);
}

} // namespace osquery
