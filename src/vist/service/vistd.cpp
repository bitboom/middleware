/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/dynamic-loader.hpp>
#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/policy/api.hpp>
#include <vist/policy/policy-manager.hpp>
#include <vist/rmi/gateway.hpp>

#include <vist/table/dynamic-table.hpp>
#include <vist/table/policy/policy-admin.hpp>
#include <vist/table/policy/policy.hpp>

#include <osquery/registry_interface.h>
#include <osquery/sql.h>

#include <filesystem>

namespace {
const std::string SOCK_ADDR = "/tmp/.vist";
} // anonymous namespace

namespace vist {

Vistd::Vistd()
{
	osquery::registryAndPluginInit();

	this->loadStaticTable();
	this->loadDynamicTable();

	policy::API::Admin::Enroll(DEFAULT_POLICY_ADMIN);
}

void Vistd::start()
{
	INFO(VIST) << "Vistd daemon starts.";

#ifdef TIZEN
	rmi::Gateway::ServiceType type = rmi::Gateway::ServiceType::OnDemand;
#else
	rmi::Gateway::ServiceType type = rmi::Gateway::ServiceType::General;
#endif

	rmi::Gateway gateway(SOCK_ADDR, type);
	EXPOSE(gateway, *this, &Vistd::query);

	auto& pm = policy::PolicyManager::Instance();

#ifdef TIZEN
	/// Shutdown service if timeout is occured without activated admin
	gateway.start(3000, [&pm]() -> bool { return !pm.isActivated(); });
#else
	(void)pm;
	gateway.start();
#endif

	INFO(VIST) << "Vistd daemon stopped.";
}

Rows Vistd::query(const std::string& statement)
{
	DEBUG(VIST) << "Excute query: " << statement;
	osquery::SQL sql(statement, true);
	if (!sql.ok())
		THROW(ErrCode::RuntimeError) << "Faild to execute query: " << sql.getMessageString();

	return sql.rows();
}

void Vistd::loadStaticTable()
{
	table::PolicyAdminTable::Init();
	table::PolicyTable::Init();
}

void Vistd::loadDynamicTable()
{
	for (auto& iter : std::filesystem::directory_iterator(TABLE_INSTALL_DIR)) {
		DEBUG(VIST) << "Load dynamic table: " << iter.path();
		DynamicLoader loader(iter.path());
		auto factory = loader.load<table::DynamicTable::FactoryType>("DynamicTableFactory");
		if (auto table = (*factory)(); table != nullptr) {
			table->init();
			delete table;
		} else {
			ERROR(VIST) << "Failed to load table.";
		}
	}
}

} // namespace vist
