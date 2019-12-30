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

#include "vistd.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/policy/policy-manager.hpp>
#include <vist/rmi/gateway.hpp>

#include <osquery/registry_interface.h>
#include <osquery/sql.h>

namespace {
	const std::string SOCK_ADDR = "/tmp/.vist";
} // anonymous namespace

namespace vist {

Vistd::Vistd()
{
	osquery::registryAndPluginInit();
}

void Vistd::start()
{
	INFO(VIST) << "Vistd daemon starts.";

	policy::PolicyManager::Instance();

	rmi::Gateway gateway(SOCK_ADDR);
	EXPOSE(gateway, *this, &Vistd::query);

	gateway.start();

	INFO(VIST) << "Vistd daemon stopped.";
}

Rows Vistd::query(const std::string& statement)
{
	osquery::SQL sql(statement, true);
	if (!sql.ok())
		THROW(ErrCode::RuntimeError) << "Faild to execute query: " << sql.getMessageString();

	return std::move(sql.rows());
}

} // namespace vist
