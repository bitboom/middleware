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

#include <vist/database/column.hpp>
#include <vist/database/statement.hpp>
#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/query-builder.hpp>

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
	database(std::make_shared<database::Connection>(path))
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
		pd.ivalue = std::string(stmt.getColumn(1));
		DEBUG(VIST) << "Defined policy:" << pd.name;
		this->definitions.emplace(pd.name, std::move(pd));
	}

	DEBUG(VIST) << definitions.size() << "-policies synced.";
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
		pa.value = std::string(stmt.getColumn(2));
		this->activatedPolicies.emplace(pa.policy, std::move(pa));
	}

	DEBUG(VIST) << activatedPolicies.size() << "-activated-policies synced.";
}

std::string PolicyStorage::getScript(const std::string& name)
{
	std::string path = SCRIPT_BASE + "/" + name + ".sql";
	std::ifstream is(path);
	if (is.fail())
		THROW(ErrCode::LogicError) << "Failed to open script: " << path;

	std::istreambuf_iterator<char> begin(is), end;
	auto content = std::string(begin, end);
	if (content.empty())
		THROW(ErrCode::LogicError) << "Failed to read script: " << path;

	return content;
}

void PolicyStorage::define(const std::string& policy, const PolicyValue& ivalue)
{
	if (this->definitions.find(policy) != this->definitions.end()) {
		INFO(VIST) << "Policy is already defined: " << policy;
		return;
	}

	PolicyDefinition pd;
	pd.name = policy;
	pd.ivalue = ivalue.dump();

	std::string query = polDefinitionTable.insert(&PolicyDefinition::name,
												  &PolicyDefinition::ivalue);
	database::Statement stmt(*database, query);
	stmt.bind(1, pd.name);
	stmt.bind(2, pd.ivalue);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << "Failed to define policy: " << pd.name;

	INFO(VIST) << "Policy defined >> name: " << pd.name << ", ivalue" << pd.ivalue;
	this->definitions.emplace(pd.name, std::move(pd));
}

void PolicyStorage::enroll(const std::string& name)
{
	INFO(VIST) << "Enroll admin: " << name;
	if (std::find(admins.begin(), admins.end(), name) != admins.end()) {
		ERROR(VIST) << "Admin is aleady enrolled.: " << name;
		return;
	}

	std::string query = adminTable.insert(&Admin::name);
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << "Failed to enroll admin: " << name;

	admins.push_back(name);
	/// PolicyActivated is triggered by enrolling admin.
	syncPolicyActivated();

	int count = activatedPolicies.size() / admins.size();
	INFO(VIST) << "Admin[" << name << "] manages "
			   << std::to_string(count) << "-policies.";
}

void PolicyStorage::disenroll(const std::string& name)
{
	if (name == DEFAULT_ADMIN_PATH)
		THROW(ErrCode::RuntimeError) << "Cannot disenroll default admin.";

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
		THROW(ErrCode::RuntimeError) << "Failed to disenroll admin: " << name;
}

void PolicyStorage::update(const std::string& admin,
						   const std::string& policy,
						   const PolicyValue& value)
{
	DEBUG(VIST) << "Policy-update is called by admin: " << admin
				<< ", about: " << policy << ", value: " << value.dump();

	if (std::find(this->admins.begin(), this->admins.end(), admin) == this->admins.end())
		THROW(ErrCode::LogicError) << "Not exist admin: " << admin;

	if (this->definitions.find(policy) == this->definitions.end())
		THROW(ErrCode::LogicError) << "Not exist policy: " << policy;

	std::string query = polActivatedTable.update(&PolicyActivated::value)
										 .where(expr(&PolicyActivated::admin) == admin &&
										  expr(&PolicyActivated::policy) == policy);
	database::Statement stmt(*this->database, query);
	stmt.bind(1, value.dump());
	stmt.bind(2, admin);
	stmt.bind(3, policy);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << "Failed to update policy:" << policy;

	/// TODO: Fix to sync without db i/o
	this->syncPolicyActivated();
}

PolicyValue PolicyStorage::strictest(const std::shared_ptr<PolicyModel>& policy)
{
	if (this->definitions.find(policy->getName()) == this->definitions.end())
		THROW(ErrCode::LogicError) << "Not exist policy: " << policy->getName();

	if (this->activatedPolicies.size() == 0) {
		INFO(VIST) << "There is no enrolled admin. Return policy initial value.";
		return policy->getInitial();
	}

	std::shared_ptr<PolicyValue> strictestPtr = nullptr;
	auto range = activatedPolicies.equal_range(policy->getName());
	for (auto iter = range.first; iter != range.second; iter++) {
		DEBUG(VIST) << "Admin: " << iter->second.admin << ", "
					<< "Policy: " << iter->second.policy  << ", "
					<< "Value: " << iter->second.value;

		if (strictestPtr == nullptr) {
			strictestPtr = std::make_shared<PolicyValue>(iter->second.value, true);
		} else {
			if (policy->compare(*strictestPtr, PolicyValue(iter->second.value, true)) > 0)
				strictestPtr.reset(new PolicyValue(iter->second.value, true));
		}
	}

	if (strictestPtr == nullptr)
		THROW(ErrCode::RuntimeError) << "Not exist managed policy: " << policy;

	DEBUG(VIST) << "The strictest value of [" << policy->getName()
				<< "] is " << strictestPtr->dump();

	return std::move(*strictestPtr);
}

const std::vector<std::string>& PolicyStorage::getAdmins() const noexcept
{
	return admins;
}

} // namespace policy
} // namespace vist
