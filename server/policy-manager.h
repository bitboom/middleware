/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __DPM_POLICY_MANAGER_H__
#define __DPM_POLICY_MANAGER_H__

#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>

#include "datasource.h"
#include "client-manager.h"

template<typename DataType>
struct PolicyComparator {
	typedef std::function<bool(DataType&, DataType&)> type;
};

class ManagedPolicy {
public:
	ManagedPolicy() :
		storage(0)
	{
	}

	virtual ~ManagedPolicy()
	{
		if (storage != nullptr) {
			delete storage;
		}
	}

	template<typename DataType>
	bool set(DataSource& datasource, const std::string& admin, uid_t domain, const DataType& value);

	template<typename DataType>
	DataType get(DataSource& datasource, uid_t domain);

	void declare(DataSource& datasource)
	{
		std::lock_guard<std::recursive_mutex> lock(updateLock);
		return storage->declare(datasource);
	}

	bool enforce(DataSource& datasource, uid_t domain)
	{
		std::lock_guard<std::recursive_mutex> lock(updateLock);
		return storage->enforce(datasource, domain);
	}

	virtual void apply(DataSource& datasource, const std::vector<uid_t>& domainList) = 0;

protected:
	class AbstractPolicyStorage {
	public:
		virtual ~AbstractPolicyStorage() {}
		virtual void declare(DataSource& source) = 0;
		virtual bool enforce(DataSource& source, uid_t domain) = 0;
	};

	ManagedPolicy(AbstractPolicyStorage* ptr) :
		storage(ptr)
	{
	}

	template<typename DataType>
	class PolicyStorage : public AbstractPolicyStorage {
	public:
		typedef DataType valueType;

		PolicyStorage(const std::string& rname, const DataType& value);
		PolicyStorage(const std::string& rname, const DataType& value,
					  const typename PolicyComparator<DataType>::type& pred);

		bool insert(uid_t domain, const DataType& value);
		bool remove(uid_t domain);
		bool compare(DataType& v1, DataType& v2);

		DataType get(DataSource& datasource, uid_t domain);
		bool set(DataSource& datasource, const std::string& admin, uid_t domain, const DataType& value);

	protected:
		int id;
		std::string name;
		DataType reference;
		std::unordered_map<uid_t, DataType> current;

		typename PolicyComparator<DataType>::type comparator;

	private:
		static std::atomic<unsigned int> sequence;
	};

	std::recursive_mutex updateLock;
	AbstractPolicyStorage* storage;
};

template<typename DataType>
bool ManagedPolicy::set(DataSource& datasource, const std::string& admin, uid_t domain, const DataType& value)
{
	std::lock_guard<std::recursive_mutex> lock(updateLock);
	return static_cast<PolicyStorage<DataType> *>(storage)->set(datasource, admin, domain, value);
}

template<typename DataType>
DataType ManagedPolicy::get(DataSource& datasource, uid_t domain)
{
	std::lock_guard<std::recursive_mutex> lock(updateLock);
	return static_cast<PolicyStorage<DataType> *>(storage)->get(datasource, domain);
}

template<typename DataType>
ManagedPolicy::PolicyStorage<DataType>::PolicyStorage(const std::string& rname, const DataType& value) :
	name(rname), reference(value)
{
	bool defaultComparator = [value](decltype(value)& v1, decltype(value)& v2) {
		return v1 < v2 ? false : true;
	};

	comparator = std::move(defaultComparator);
	id = this->sequence++;
}

template<typename DataType>
ManagedPolicy::PolicyStorage<DataType>::PolicyStorage(const std::string& rname, const DataType& value,
													  const typename PolicyComparator<DataType>::type& pred) :
	name(rname), reference(value), comparator(pred)
{
	id = this->sequence++;
}

template<typename DataType>
bool ManagedPolicy::PolicyStorage<DataType>::insert(uid_t domain, const DataType& value)
{
	int count = current.count(domain);
	if ((count == 0) || (current[domain] != value)) {
		current[domain] = value;
		return true;
	}

	return false;
}

template<typename DataType>
bool ManagedPolicy::PolicyStorage<DataType>::remove(uid_t domain)
{
	if (current.count(domain)) {
		current.erase(domain);
		return true;
	}

	return false;
}

template<typename DataType>
bool ManagedPolicy::PolicyStorage<DataType>::compare(DataType& v1, DataType& v2)
{
	return comparator(v1, v2);
}

template<typename DataType>
DataType ManagedPolicy::PolicyStorage<DataType>::get(DataSource& datasource, uid_t domain)
{
	if (current.count(domain)) {
		return current[domain];
	}

	return reference;
}

template<typename DataType>
bool ManagedPolicy::PolicyStorage<DataType>::set(DataSource& datasource, const std::string& admin, uid_t domain, const DataType& value)
{
	std::string selectQuery = "SELECT id FROM admin WHERE pkg = ? AND uid = ?";
	database::Statement stmt0(datasource, selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(domain));
	if (!stmt0.step()) {
		throw runtime::Exception("Unknown device admin client: " + admin);
	}

	int aid = stmt0.getColumn(0).getInt();

	std::string updateQuery = "UPDATE managed_policy SET value = ? WHERE pid = ? AND aid = ?";
	database::Statement stmt(datasource, updateQuery);
	stmt.bind(1, value);
	stmt.bind(2, id);
	stmt.bind(3, aid);
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to update policy");
	}

	return enforce(datasource, domain);
}

class PolicyManager {
public:
	PolicyManager(const std::string& base);
	~PolicyManager();

	PolicyManager(const PolicyManager&) = delete;
	PolicyManager& operator=(const PolicyManager&) = delete;

	void apply();

	int prepareStorage(const std::string& admin, uid_t uid);
	int removeStorage(const std::string& admin, uid_t uid);

	template<typename DataType>
	bool setGlobalPolicy(const DeviceAdministrator& admin, const std::string& policyName, const DataType& value);

	template<typename DataType>
	bool setUserPolicy(const DeviceAdministrator& admin, const std::string& policyName, const DataType& value);

	template<typename DataType>
	bool setPolicy(const DeviceAdministrator& admin, const std::string& policyName, const DataType& value);

	template<typename DataType>
	DataType getGlobalPolicy(const std::string& policyName);

	template<typename DataType>
	DataType getUserPolicy(const std::string& policyName, uid_t uid);

private:
	void initializeStorage();
	std::vector<uid_t> getManagedDomainList();

private:
	database::Connection connection;
	static std::unordered_map<std::string, ManagedPolicy *> managedPolicyMap;
};

template<typename DataType>
DataType PolicyManager::getGlobalPolicy(const std::string& name)
{
	if (!managedPolicyMap.count(name)) {
		throw runtime::Exception("Unknown policy: " + name);
	}

	ManagedPolicy* policy = managedPolicyMap[name];
	return policy->get<DataType>(connection, 0);
}

template<typename DataType>
DataType PolicyManager::getUserPolicy(const std::string& name, uid_t domain)
{
	if (!managedPolicyMap.count(name)) {
		throw runtime::Exception("Unknown policy: " + name);
	}

	ManagedPolicy* policy = managedPolicyMap.at(name);
	return policy->get<DataType>(connection, domain);
}

template<typename DataType>
bool PolicyManager::setUserPolicy(const DeviceAdministrator& admin,
								  const std::string& policyName, const DataType& value)
{
	if (!managedPolicyMap.count(policyName)) {
		throw runtime::Exception("Unknown policy: " + policyName);
	}

	ManagedPolicy* policy = managedPolicyMap.at(policyName);
	return policy->set<DataType>(connection, admin.getName(), admin.getUid(), value);
}

template<typename DataType>
bool PolicyManager::setGlobalPolicy(const DeviceAdministrator& admin,
									const std::string& policyName, const DataType& value)
{
	if (!managedPolicyMap.count(policyName)) {
		throw runtime::Exception("Unknown policy: " + policyName);
	}

	ManagedPolicy* policy = managedPolicyMap.at(policyName);
	return policy->set<DataType>(connection, admin.getName(), admin.getUid(), value);
}

template<typename DataType>
bool PolicyManager::setPolicy(const DeviceAdministrator& admin, const std::string& policyName, const DataType& value)
{
	if (!managedPolicyMap.count(policyName)) {
		throw runtime::Exception("Unknown policy: " + policyName);
	}

	ManagedPolicy* policy = managedPolicyMap.at(policyName);
	return policy->set<DataType>(connection, admin.getName(), admin.getUid(), value);
}

#endif //__DPM_POLICY_MANAGER_H__
