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

#include "policy-storage.hpp"

#include <vist/logger.hpp>
#include <vist/query-builder.hpp>

#include <klay/db/column.h>
#include <klay/db/statement.h>
#include <klay/exception.h>

#include <algorithm>
#include <fstream>

using namespace vist::tsqb;
using namespace vist::policy::schema;

namespace {

auto adminTable = make_table("ADMIN", make_column("name", &Admin::name));

auto polActivatedTable = make_table("POLICY_ACTIVATED",
									make_column("admin", &PolicyActivated::admin),
									make_column("policy", &PolicyActivated::policy),
									make_column("value", &PolicyActivated::value));

auto polDefinitionTable = make_table("POLICY_DEFINITION",
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
	DEBUG(VIST) << "Sync policy storage to cache object.";
	syncAdmin();
	syncPolicyActivated();
	syncPolicyDefinition();
}

void PolicyStorage::syncPolicyDefinition()
{
	this->definitions.clear();
	std::string query = polDefinitionTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyDefinition pd;
		pd.name = std::string(stmt.getColumn(0));
		pd.ivalue = stmt.getColumn(1);
		DEBUG(VIST) << "Defined policy:" << pd.name;
		this->definitions.emplace(pd.name, std::move(pd));
	}

	DEBUG(VIST) << definitions.size() << "- policies synced.";
}

void PolicyStorage::syncAdmin()
{
	this->admins.clear();
	std::string query = adminTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step())
		this->admins.emplace_back(std::string(stmt.getColumn(0)));

	DEBUG(VIST) << admins.size() << "-admins synced.";
}

void PolicyStorage::syncPolicyActivated()
{
	this->activatedPolicies.clear();
	std::string query = polActivatedTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyActivated pa;
		pa.admin = std::string(stmt.getColumn(0));
		pa.policy = std::string(stmt.getColumn(1));
		pa.value = stmt.getColumn(2);
		this->activatedPolicies.emplace(pa.policy, std::move(pa));
	}

	DEBUG(VIST) << activatedPolicies.size() << "- activated-policies synced.";
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
		INFO(VIST) << "Policy is already defined: " << policy;
		return;
	}

	PolicyDefinition pd;
	pd.name = policy;
	pd.ivalue = ivalue;

	std::string query = polDefinitionTable.insert(&PolicyDefinition::name,
												  &PolicyDefinition::ivalue);
	database::Statement stmt(*database, query);
	stmt.bind(1, pd.name);
	stmt.bind(2, pd.ivalue);
	if (!stmt.exec())
		throw std::runtime_error("Failed to define policy: " + pd.name);
}

void PolicyStorage::enroll(const std::string& name)
{
	INFO(VIST) << "Enroll admin: " << name;
	if (std::find(admins.begin(), admins.end(), name) != admins.end()) {
		ERROR(VIST) << "Admin is aleady enrolled.: " << name;
		return;
	}

	std::string query = adminTable.insert(&Admin::name);
	DEBUG(VIST) << "Enroll admin query statement: " << query;
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	if (!stmt.exec())
		throw std::runtime_error("Failed to enroll admin: " + name);

	admins.push_back(name);
	/// PolicyActivated is triggered by enrolling admin.
	syncPolicyActivated();

	int count = activatedPolicies.size() / admins.size();
	INFO(VIST) << "Admin[" << name << "] manages "
			   << std::to_string(count) << "-policies.";
}

void PolicyStorage::disenroll(const std::string& name)
{
	INFO(VIST) << "Disenroll admin: " << name;
	auto iter = std::find(admins.begin(), admins.end(), name);
	if (iter == admins.end()) {
		ERROR(VIST) << "Not exist admin: " << name;
		return;
	} else {
		admins.erase(iter);
	}

	std::string query = adminTable.remove().where(expr(&Admin::name) == name);
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	if (!stmt.exec())
		throw std::runtime_error("Failed to disenroll admin: " + name);
}

void PolicyStorage::update(const std::string& admin,
						   const std::string& policy,
						   const PolicyValue& value)
{
	DEBUG(VIST) << "Policy-update is called by admin: " << admin
				<< ", about: " << policy << ", value: " << std::to_string(value);

	if (std::find(admins.begin(), admins.end(), admin) == admins.end())
		throw std::runtime_error("Not exist admin: " + admin);

	if (definitions.find(policy) == definitions.end())
		throw std::runtime_error("Not exist policy: " + policy);

	int policyValue = value;
	std::string query = polActivatedTable.update(&PolicyActivated::value)
									   .where(expr(&PolicyActivated::admin) == admin &&
											  expr(&PolicyActivated::policy) == policy);
	database::Statement stmt(*database, query);
	stmt.bind(1, policyValue);
	stmt.bind(2, admin);
	stmt.bind(3, policy);
	if (!stmt.exec())
		throw runtime::Exception("Failed to update policy:" + policy);

	syncPolicyActivated();
}

PolicyValue PolicyStorage::strictest(const std::string& policy)
{
	if (definitions.find(policy) == definitions.end())
		throw std::runtime_error("Not exist policy: " + policy);

	// There is no enrolled admins.
	if (activatedPolicies.size() == 0)
		return PolicyValue(definitions[policy].ivalue);

	std::shared_ptr<PolicyValue> strictestPtr = nullptr;
	auto range = activatedPolicies.equal_range(policy);
	for (auto iter = range.first; iter != range.second; iter++) {
		int value = iter->second.value;
		if (strictestPtr == nullptr)
			strictestPtr = std::make_shared<PolicyValue>(value);
		else
			strictestPtr->value = (*strictestPtr < value) ? strictestPtr->value : value;

		DEBUG(VIST) << "The strictest of policy[" << policy
					<< "] : " + std::to_string(strictestPtr->value);
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

const std::vector<std::string>& PolicyStorage::getAdmins() const noexcept
{
	return admins;
}

} // namespace policy
} // namespace vist
