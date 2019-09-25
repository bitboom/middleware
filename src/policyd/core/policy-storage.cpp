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

#include "policy-storage.h"
#include "logger.h"

#include <klay/db/column.h>
#include <klay/db/query-builder.h>
#include <klay/db/statement.h>
#include <klay/exception.h>

using namespace query_builder;
using namespace policyd::schema;

namespace {

auto _admin = make_table("admin",
						 make_column("id", &Admin::id),
						 make_column("pkg", &Admin::pkg),
						 make_column("uid", &Admin::uid),
						 make_column("key", &Admin::key),
						 make_column("removable", &Admin::removable));

auto _managedPolicy = make_table("managed_policy",
								 make_column("id", &ManagedPolicy::id),
								 make_column("aid", &ManagedPolicy::aid),
								 make_column("pid", &ManagedPolicy::pid),
								 make_column("value", &ManagedPolicy::value));

auto _policyDefinition = make_table("policy_definition",
									make_column("id", &PolicyDefinition::id),
									make_column("scope", &PolicyDefinition::scope),
									make_column("name", &PolicyDefinition::name),
									make_column("ivalue", &PolicyDefinition::ivalue));

auto _dpm = make_database("dpm", _admin, _managedPolicy, _policyDefinition);

} // anonymous namespace

namespace policyd {

PolicyStorage::PolicyStorage(const std::string& path) :
	database(std::make_shared<database::Connection>(path,
													database::Connection::ReadWrite |
													database::Connection::Create))
{
	sync();
}

void PolicyStorage::sync()
{
	DEBUG(DPM, "Sync policy storage to cache object.");
	syncPolicyDefinition();
	syncAdmin();
	syncManagedPolicy();
}

void PolicyStorage::syncPolicyDefinition()
{
	std::string query = _policyDefinition.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyDefinition pd;
		pd.id = stmt.getColumn(0);
		pd.scope = stmt.getColumn(1);
		pd.name = std::string(stmt.getColumn(2));
		pd.ivalue = stmt.getColumn(3);
		DEBUG(DPM, "Sync policy:" + pd.name);
		this->definitions.emplace(pd.name, std::move(pd));
	}
}

void PolicyStorage::syncAdmin()
{
	std::string query = _admin.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		Admin admin;
		admin.id = stmt.getColumn(0);
		admin.pkg = std::string(stmt.getColumn(1));
		admin.uid = stmt.getColumn(2);
		admin.key = std::string(stmt.getColumn(3));
		admin.removable = stmt.getColumn(4);
		this->admins.emplace_back(std::move(admin));
	}
}

void PolicyStorage::syncManagedPolicy()
{
	std::string query = _managedPolicy.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		ManagedPolicy mp;
		mp.id = stmt.getColumn(0);
		mp.aid = stmt.getColumn(1);
		mp.pid = stmt.getColumn(2);
		mp.value = stmt.getColumn(3);
		this->managedPolicies.emplace_back(std::move(mp));
	}
}

} // namespace policyd
