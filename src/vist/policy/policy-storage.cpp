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

#include "policy-storage.hpp"

#include <vist/database/column.hpp>
#include <vist/database/statement.hpp>
#include <vist/exception.hpp>
#include <vist/logger.hpp>
#include <vist/query-builder.hpp>

#include <algorithm>
#include <fstream>

namespace {

const std::string SCRIPT_BASE = SCRIPT_INSTALL_DIR;
const std::string SCRIPT_CREATE_SCHEMA  = "create-schema";

} // anonymous namespace

namespace vist {
namespace policy {

PolicyStorage::PolicyStorage(const std::string& path) :
	database(std::make_shared<database::Connection>(path))
{
	database->exec("PRAGMA foreign_keys = ON;");
	database->transactionBegin();
	database->exec(getScript(SCRIPT_CREATE_SCHEMA));
	database->transactionEnd();

	sync();
}

void PolicyStorage::sync()
{
	DEBUG(VIST) << "Sync policy storage to cache object.";
	syncAdmin();
	syncPolicyManaged();
	syncPolicyDefinition();
}

void PolicyStorage::syncPolicyDefinition()
{
	this->definitions.clear();
	std::string query = schema::PolicyDefinitionTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyDefinition pd = { std::string(stmt.getColumn(0)), std::string(stmt.getColumn(1)) };
		DEBUG(VIST) << "Defined policy:" << pd.name;
		this->definitions.emplace(pd.name, std::move(pd));
	}

	DEBUG(VIST) << definitions.size() << "-policies synced.";
}

void PolicyStorage::syncAdmin()
{
	this->admins.clear();
	std::string query = schema::AdminTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		Admin admin = { std::string(stmt.getColumn(0)), stmt.getColumn(1).getInt() };
		this->admins.emplace(admin.name, std::move(admin));
	}

	DEBUG(VIST) << admins.size() << "-admins synced.";
}

void PolicyStorage::syncPolicyManaged()
{
	this->managedPolicies.clear();
	std::string query = schema::PolicyManagedTable.selectAll();
	database::Statement stmt(*database, query);

	while (stmt.step()) {
		PolicyManaged pa;
		pa.admin = std::string(stmt.getColumn(0));
		pa.policy = std::string(stmt.getColumn(1));
		pa.value = std::string(stmt.getColumn(2));
		this->managedPolicies.emplace(pa.policy, std::move(pa));
	}

	DEBUG(VIST) << managedPolicies.size() << "-managed-policies synced.";
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
		DEBUG(VIST) << "Policy is already defined: " << policy;
		return;
	}

	PolicyDefinition pd = { policy, ivalue.dump() };

	std::string query =
		schema::PolicyDefinitionTable.insert(PolicyDefinition::Name = pd.name,
											 PolicyDefinition::Ivalue = pd.ivalue);
	database::Statement stmt(*database, query);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << stmt.getErrorMessage();

	INFO(VIST) << "Policy defined >> name: " << pd.name << ", ivalue: " << pd.ivalue;
	this->definitions.emplace(pd.name, std::move(pd));
}

void PolicyStorage::enroll(const std::string& name)
{
	INFO(VIST) << "Enroll admin: " << name;
	if (this->admins.find(name) != this->admins.end()) {
		WARN(VIST) << "Admin is already enrolled.: " << name;
		return;
	}

	/// Make admin deactivated as default.
	Admin admin = {name , 0};

	std::string query = schema::AdminTable.insert(Admin::Name = admin.name,
												  Admin::Activated = admin.activated);
	database::Statement stmt(*database, query);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << stmt.getErrorMessage();

	this->admins.emplace(admin.name, std::move(admin));

	/// PolicyManaged is triggered by enrolling admin.
	syncPolicyManaged();

	int count = this->managedPolicies.size() / this->admins.size();
	INFO(VIST) << "Admin[" << name << "] manages "
			   << std::to_string(count) << "-policies.";
}

void PolicyStorage::disenroll(const std::string& name)
{
	if (name == DEFAULT_ADMIN_PATH)
		THROW(ErrCode::RuntimeError) << "Cannot disenroll default admin.";

	INFO(VIST) << "Disenroll admin: " << name;
	if (this->admins.find(name) == this->admins.end()) {
		ERROR(VIST) << "Not exist admin: " << name;
		return;
	} else {
		this->admins.erase(name);
	}

	std::string query = schema::AdminTable.remove().where(Admin::Name == name);
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << stmt.getErrorMessage();

	/// TODO: add TC
	this->syncPolicyManaged();
}

void PolicyStorage::activate(const std::string& admin, bool state)
{
	if (this->admins.find(admin) == this->admins.end())
		THROW(ErrCode::LogicError) << "Not exist admin: " << admin;

	DEBUG(VIST) << "Activate admin: " << admin;
	std::string query = schema::AdminTable.update(Admin::Activated = static_cast<int>(state))
										  .where(Admin::Name == admin);
	database::Statement stmt(*this->database, query);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << stmt.getErrorMessage();

	this->admins[admin].activated = state;
	INFO(VIST) << "Admin[" << admin << "]'s activated value is set: " << state; 
}

bool PolicyStorage::isActivated(const std::string& admin)
{
	if (this->admins.find(admin) == this->admins.end())
		THROW(ErrCode::LogicError) << "Not exist admin: " << admin;

	return this->admins[admin].activated;
}

bool PolicyStorage::isActivated()
{
	for (const auto& admin : this->admins)
		if (admin.second.activated)
			return true;

	return false;
}

void PolicyStorage::update(const std::string& admin,
						   const std::string& policy,
						   const PolicyValue& value)
{
	DEBUG(VIST) << "Policy-update is called by admin: " << admin
				<< ", about: " << policy << ", value: " << value.dump();

	if (this->admins.find(admin) == this->admins.end())
		THROW(ErrCode::LogicError) << "Not exist admin: " << admin;

	if (this->definitions.find(policy) == this->definitions.end())
		THROW(ErrCode::LogicError) << "Not exist policy: " << policy;

	std::string query = schema::PolicyManagedTable.update(PolicyManaged::Value = value.dump())
												  .where(PolicyManaged::Admin == admin &&
														 PolicyManaged::Policy == policy);
	database::Statement stmt(*this->database, query);
	if (!stmt.exec())
		THROW(ErrCode::RuntimeError) << stmt.getErrorMessage();

	/// TODO: Fix to sync without db i/o
	this->syncPolicyManaged();
}

PolicyValue PolicyStorage::strictest(const std::shared_ptr<PolicyModel>& policy)
{
	if (this->definitions.find(policy->getName()) == this->definitions.end())
		THROW(ErrCode::LogicError) << "Not exist policy: " << policy->getName();

	if (this->managedPolicies.size() == 0) {
		INFO(VIST) << "There is no enrolled admin. Return policy initial value.";
		return policy->getInitial();
	}

	std::shared_ptr<PolicyValue> strictestPtr = nullptr;
	auto range = managedPolicies.equal_range(policy->getName());
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

std::unordered_map<std::string, int> PolicyStorage::getAdmins() const noexcept
{
	std::unordered_map<std::string, int> ret;
	for (const auto& admin : this->admins)
		ret[admin.second.name] = admin.second.activated;

	return ret;
}

} // namespace policy
} // namespace vist
