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

#include "vistd.h"
#include "ipc/server.h"

#include <stdexcept>

#include <osquery/registry.h>
#include <osquery/sql.h>
#include <osquery/status.h>

#define QUERY_RET_TYPE std::vector<std::map<std::string, std::string>>

namespace {
	const std::string SOCK_ADDR = "/tmp/.vistd";
} // anonymous namespace

namespace vist {

Vistd::Vistd()
{
	osquery::registryAndPluginInit();
}

void Vistd::start()
{
	auto& server = ipc::Server::Instance(SOCK_ADDR);

	server->expose(this, "", (QUERY_RET_TYPE)(Vistd::query)(std::string));
	server->start();
}

Rows Vistd::query(const std::string& statement)
{
	osquery::SQL sql(statement, true);
	if (!sql.ok())
		throw std::runtime_error("Faild to execute query: " + sql.getMessageString());

	return std::move(sql.rows());
}

} // namespace vist
