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

#include "query.hpp"

#include <vist/logger.hpp>
#include <vist/rmi/remote.hpp>

namespace {
const std::string SOCK_ADDR = "/tmp/.vist";
} // anonymous namespace

namespace vist {

Rows Query::Execute(const std::string& statement)
{
	INFO(VIST_CLIENT) << "Query execution: " << statement;
	rmi::Remote remote(SOCK_ADDR);

	auto query = REMOTE_METHOD(remote, &Vistd::query);
	auto rows = query.invoke<Rows>(statement);

	DEBUG(VIST_CLIENT) << "Row's size: " << rows.size();
	for (const auto& row : rows)
		for (const auto& col : row)
			DEBUG(VIST_CLIENT) << col.first << ", " << col.second;

	return rows;
}

} // namespace vist
