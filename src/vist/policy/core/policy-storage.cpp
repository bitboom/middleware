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

#include <vist/common/audit/logger.h>

#include <klay/db/column.h>
#include <klay/db/query-builder.h>
#include <klay/db/statement.h>
#include <klay/exception.h>

#include <fstream>

using namespace query_builder;
using namespace vist::policy::schema;

namespace {

auto adminTable = make_table("admin",
							 make_column("id", &Admin::id),
							 make_column("pkg", &Admin::pkg),
							 make_column("key", &Admin::key),
							 make_column("removable", &Admin::removable));

auto managedPolTable = make_table("managed_policy",
								  make_column("id", &ManagedPolicy::id),
								  make_column("aid", &ManagedPolicy::aid),
								  make_column("pid", &ManagedPolicy::pid),
								  make_column("value", &ManagedPolicy::value));

auto definitionTable = make_table("policy_definition",
								  make_column("id", &PolicyDefinition::id),
								  make_column("name", &PolicyDefinition::name),
								  make_column("ivalue", &PolicyDefinition::ivalue));

const std::string SCRIPT_BASE = SCRIPT_INSTALL_DIR;
const std::string SCRIPT_CREATE_SCHEMA  = "create-schema";

} // anonymous namespace

namespace vist {
namespace policy {

PolicyStorage::PolicyStorage(const std::string& path) :
	database(std::make_shared<database::Connection>(path,
													database::Connection::ReadWrite |
													database::Connection::Create))
{
	database->exec("PRAGMA foreign_keys = ON;");
	database->exec(getScript(SCRIPT_CREATE_SCHEMA));

	sync();
}

void PolicyStorage::sync()
{
	DEBUG(VIST, "Sync policy storage to cache object.");
	syncPolicyDefinition();
	syncAdmin();
	syncManagedPolicy();
}

void PolicyStorage::syncPolicyDefinition()
{
	this->definitions.clear();
	std::string query = definitionTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyDefinition pd;
		pd.id = stmt.getColumn(0);
		pd.name = std::string(stmt.getColumn(1));
		pd.ivalue = stmt.getColumn(2);
		DEBUG(VIST, "Defined policy:" + pd.name);
		this->definitions.emplace(pd.name, std::move(pd));
	}
}

void PolicyStorage::syncAdmin()
{
	this->admins.clear();
	std::string query = adminTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		Admin admin;
		admin.id = stmt.getColumn(0);
		admin.pkg = std::string(stmt.getColumn(1));
		admin.key = std::string(stmt.getColumn(2));
		admin.removable = stmt.getColumn(3);

		this->admins.emplace(admin.pkg, std::move(admin));
	}
}

void PolicyStorage::syncManagedPolicy()
{
	this->managedPolicies.clear();
	std::string query = managedPolTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		ManagedPolicy mp;
		mp.id = stmt.getColumn(0);
		mp.aid = stmt.getColumn(1);
		mp.pid = stmt.getColumn(2);
		mp.value = stmt.getColumn(3);
		this->managedPolicies.emplace(mp.pid, std::move(mp));
	}
}

std::string PolicyStorage::getScript(const std::string& name)
{
	std::string path = SCRIPT_BASE + "/" + name + ".sql";
	std::ifstream is(path);
	if (is.fail())
		throw std::invalid_argument("Failed to open script: " + path); 

	std::istreambuf_iterator<char> begin(is), end;
	auto content = std::string(begin, end);
	if (content.empty())
		throw std::runtime_error("Failed to read script: " + path); 

	return content;
}

void PolicyStorage::define(const std::string& policy, int ivalue)
{
	if (definitions.find(policy) != definitions.end()) {
		INFO(VIST, "Policy is already defined: " + policy);
		return;
	}

	PolicyDefinition pd;
	pd.name = policy;
	pd.ivalue = ivalue;

	std::string insertQuery = definitionTable.insert(&PolicyDefinition::name,
													 &PolicyDefinition::ivalue);
	database::Statement stmt(*database, insertQuery);
	stmt.bind(1, pd.name);
	stmt.bind(2, pd.ivalue);
	if (!stmt.exec())
		throw std::runtime_error("Failed to define policy: " + pd.name);
}

void PolicyStorage::enroll(const std::string& name)
{
	INFO(VIST, "Enroll admin: " + name);
	if (admins.find(name) != admins.end()) {
		ERROR(VIST, "Admin is aleady enrolled.: " + name);
		return;
	}

	Admin admin;
	admin.pkg = name;
	admin.key = "Not supported";
	admin.removable = true;

	std::string insertQuery = adminTable.insert(&Admin::pkg,
												&Admin::key,
												&Admin::removable);
	database::Statement stmt(*database, insertQuery);
	stmt.bind(1, admin.pkg);
	stmt.bind(2, admin.key);
	stmt.bind(3, admin.removable);
	if (!stmt.exec())
		throw std::runtime_error("Failed to enroll admin: " + admin.pkg);

	/// Sync admin for getting admin ID.
	syncAdmin();
	/// ManagedPolicy is triggered by enrolling admin.
	syncManagedPolicy();

	int count = managedPolicies.size() / admins.size();
	INFO(VIST, "Admin[" + name + "] manages " + std::to_string(count) + "-policies.");
}

void PolicyStorage::disenroll(const std::string& name)
{
	INFO(VIST, "Disenroll admin: " + name);
	if (admins.find(name) == admins.end()) {
		ERROR(VIST, "Not exist admin: " + name);
		return;
	} else {
		admins.erase(name);
	}

	std::string query = adminTable.remove().where(expr(&Admin::pkg) == name);
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	if (!stmt.exec())
		throw std::runtime_error("Failed to disenroll admin: " + name);
}

void PolicyStorage::update(const std::string& admin,
						   const std::string& policy,
						   const PolicyValue& value)
{
	if (admins.find(admin) == admins.end())
		throw std::runtime_error("Not exist admin: " + admin);

	if (definitions.find(policy) == definitions.end())
		throw std::runtime_error("Not exist policy: " + policy);

	DEBUG(VIST, "Policy-update is called by admin: " + admin + ", about: " + policy +
				", value: " + std::to_string(value));

	int policyId = definitions[policy].id;
	int policyValue = value;
	int adminId = admins[admin].id;
	std::string query = managedPolTable.update(&ManagedPolicy::value)
									   .where(expr(&ManagedPolicy::pid) == policyId &&
											  expr(&ManagedPolicy::aid) == adminId);
	database::Statement stmt(*database, query);
	stmt.bind(1, policyValue);
	stmt.bind(2, policyId);
	stmt.bind(3, adminId);
	if (!stmt.exec())
		throw runtime::Exception("Failed to update policy:" + policy);

	syncManagedPolicy();
}

PolicyValue PolicyStorage::strictest(const std::string& policy)
{
	if (definitions.find(policy) == definitions.end())
		throw std::runtime_error("Not exist policy: " + policy);

	// There is no enrolled admins.
	if (managedPolicies.size() == 0)
		return PolicyValue(definitions[policy].ivalue);

	std::shared_ptr<PolicyValue> strictestPtr = nullptr;
	int policyId = definitions[policy].id;
	auto range = managedPolicies.equal_range(policyId);
	for (auto iter = range.first; iter != range.second; iter++) {
		int value = iter->second.value;
		if (strictestPtr == nullptr)
			strictestPtr = std::make_shared<PolicyValue>(value);
		else
			strictestPtr->value = (*strictestPtr < value) ? strictestPtr->value : value;

		DEBUG(VIST, "The strictest of policy[" + policy +
				    "] : " + std::to_string(strictestPtr->value));
	}

	if (strictestPtr == nullptr)
		throw std::runtime_error("Not exist managed policy: " + policy);

	return std::move(*strictestPtr);
}

std::unordered_map<std::string, PolicyValue> PolicyStorage::strictest()
{
	std::unordered_map<std::string, PolicyValue> policies;
	for (const auto& pair : definitions) {
		std::string name = pair.first;
		auto value = this->strictest(name);

		policies.emplace(std::move(name), std::move(value));
	}

	return policies;
}

std::vector<std::string> PolicyStorage::getAdmins()
{
	std::vector<std::string> admins;
	for (const auto& pair : this->admins) {
		std::string name = pair.first;
		admins.emplace_back(std::move(name));
	}

	return admins;
}

} // namespace policy
} // namespace vist
