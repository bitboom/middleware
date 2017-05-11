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

#ifndef __DPM_POLICY_MODEL_H__
#define __DPM_POLICY_MODEL_H__
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <iostream>

#include <klay/exception.h>

#include "policy-storage.h"
#include "client-manager.h"
#include "observer.h"

#include "policy-context.hxx"

/**
 * @brief This class provides primitives for event notification to clients.
 */
class BaseEnforceModel {
public:
	BaseEnforceModel(PolicyControlContext& ctxt, const std::string& name, bool createEvent = true) :
		event(name), hasEvent(createEvent), context(ctxt)
	{
		if (hasEvent) {
			context.createNotification(event);
		}
	}

	template<typename DataType>
	void notify(const DataType& value)
	{
		if (hasEvent) {
			context.notify(event, value);
		}
	}

private:
	std::string event;
	bool hasEvent;
	PolicyControlContext& context;
};

/**
 * @brief Default framework enforce model.
 */
class DefaultEnforceModel : public BaseEnforceModel {
public:
	DefaultEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	template<typename DataType>
	bool operator()(const DataType& value, uid_t domain = 0)
	{
		notify(value);
		return true;
	}
};

template<
	typename DataType,
	typename EnforceModel = DefaultEnforceModel
>
class DomainPolicy : public Observer {
public:
	typedef DataType valueType;

	DomainPolicy(PolicyControlContext& ctxt, const std::string& name);

	void set(const std::string& admin, uid_t domain, const DataType& value);
	DataType get(uid_t domain);
	void enforce(uid_t domain);

	void onEvent(const std::vector<uid_t>& domains);

protected:
	bool strictize(uid_t domain);
	void update(const std::string& admin, uid_t domain, const DataType& value);

private:
	int id;
	DataType pivot;
	std::unordered_map<uid_t, DataType> current;

	EnforceModel enforceModel;
};

template<typename DataType, typename EnforceModel>
DomainPolicy<DataType, EnforceModel>::DomainPolicy(PolicyControlContext& ctxt, const std::string& name) :
	id(0), pivot(), current(), enforceModel(ctxt, name)
{
	DataSet stmt = PolicyStorage::prepare("SELECT id, ivalue FROM policy_definition WHERE name = ?");
	stmt.bind(1, name);
	if (stmt.step()) {
		id = stmt.getColumn(0);
		DataType value(stmt.getColumn(1));
		pivot = std::move(value);
		ClientManager::addEventListener(this);
	}
}

template<typename DataType, typename EnforceModel>
void DomainPolicy<DataType, EnforceModel>::set(const std::string& admin, uid_t domain, const DataType& value)
{
	update(admin, domain, value);
	if (strictize(domain)) {
		enforceModel(current[domain], domain);
	}
}

template<typename DataType, typename EnforceModel>
DataType DomainPolicy<DataType, EnforceModel>::get(uid_t domain)
{
	if (current.count(domain)) {
		return current[domain];
	}

	return pivot;
}

template<typename DataType, typename EnforceModel>
void DomainPolicy<DataType, EnforceModel>::enforce(uid_t domain)
{
	enforceModel(current[domain], domain);
}

template<typename DataType, typename EnforceModel>
void DomainPolicy<DataType, EnforceModel>::onEvent(const std::vector<uid_t>& domains)
{
	for (auto domain: domains) {
		// Initialize domain policy value if it was not managed
		if (!current.count(domain)) {
			current[domain] = pivot;
		}
		if (strictize(domain)) {
			enforceModel(current[domain], domain);
		}
	}
}

template<typename DataType, typename EnforceModel>
bool DomainPolicy<DataType, EnforceModel>::strictize(uid_t domain)
{
	int count = 0;
	DataType strict = pivot;
	std::string query = "SELECT managed_policy.value FROM managed_policy " \
						"INNER JOIN policy_definition ON managed_policy.pid = policy_definition.id " \
						"INNER JOIN admin ON managed_policy.aid = admin.id " \
						"WHERE policy_definition.scope = 1 AND managed_policy.pid = ? AND admin.uid = ? ";

	DataSet stmt = PolicyStorage::prepare(query);
	stmt.bind(1, id);
	stmt.bind(2, static_cast<int>(domain));
	while (stmt.step()) {
		DataType value(stmt.getColumn(0));
		if (value < strict) {
			strict = std::move(value);
		}
		count++;
	}

	bool ret = current[domain] != strict;
	if (count == 0) {
		current.erase(domain);
		return ret;
	}
	if (ret) {
		current[domain] = strict;
	}

	return ret;
}

template<typename DataType, typename EnforceModel>
void DomainPolicy<DataType, EnforceModel>::update(const std::string& admin, uid_t domain, const DataType& value)
{
	std::string selectQuery = "SELECT id FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt0 = PolicyStorage::prepare(selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(domain));
	if (!stmt0.step()) {
		throw runtime::Exception("Unknown device admin client: " + admin);
	}

	int aid = stmt0.getColumn(0);

	std::string updateQuery = "UPDATE managed_policy SET value = ? WHERE pid = ? AND aid = ?";
	DataSet stmt = PolicyStorage::prepare(updateQuery);
	stmt.bind(1, value);
	stmt.bind(2, id);
	stmt.bind(3, aid);
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to update policy");
	}
}

template<
	typename DataType,
	typename EnforceModel = DefaultEnforceModel
>
class GlobalPolicy : public Observer {
public:
	typedef DataType valueType;

	GlobalPolicy(PolicyControlContext& ctxt, const std::string& name);

	void set(const std::string& admin, uid_t domain, const DataType& value);
	DataType get(uid_t domain);

	void enforce();

	void onEvent(const std::vector<uid_t>& domains);

protected:
	bool strictize();
	void update(const std::string& admin, uid_t domain, const DataType& value);

private:
	int id;
	DataType pivot;
	DataType current;

	EnforceModel enforceModel;
};

template<typename DataType, typename EnforceModel>
GlobalPolicy<DataType, EnforceModel>::GlobalPolicy(PolicyControlContext& ctxt, const std::string& name) :
	id(0), pivot(), current(), enforceModel(ctxt, name)
{
	DataSet stmt = PolicyStorage::prepare("SELECT id, ivalue FROM policy_definition WHERE name = ?");
	stmt.bind(1, name);
	if (stmt.step()) {
		id = stmt.getColumn(0);
		DataType value(stmt.getColumn(1));
		pivot = current = std::move(value);
		ClientManager::addEventListener(this);
	}
}

template<typename DataType, typename EnforceModel>
void GlobalPolicy<DataType, EnforceModel>::set(const std::string& admin, uid_t domain, const DataType& value)
{
	update(admin, domain, value);
	if (strictize()) {
		enforceModel(current);
	}
}

template<typename DataType, typename EnforceModel>
DataType GlobalPolicy<DataType, EnforceModel>::get(uid_t domain)
{
	return current;
}

template<typename DataType, typename EnforceModel>
void GlobalPolicy<DataType, EnforceModel>::enforce()
{
	enforceModel(current);
}

template<typename DataType, typename EnforceModel>
void GlobalPolicy<DataType, EnforceModel>::onEvent(const std::vector<uid_t>& domains)
{
	if (strictize()) {
		enforceModel(current);
	}
}

template<typename DataType, typename EnforceModel>
bool GlobalPolicy<DataType, EnforceModel>::strictize()
{
	int count = 0;
	DataType strictness = pivot;
	std::string query = "SELECT managed_policy.value FROM managed_policy " \
						"INNER JOIN policy_definition ON managed_policy.pid = policy_definition.id " \
						"INNER JOIN admin ON managed_policy.aid = admin.id " \
						"WHERE policy_definition.scope = 0 AND managed_policy.pid = ? ";

	DataSet stmt = PolicyStorage::prepare(query);
	stmt.bind(1, id);
	while (stmt.step()) {
		DataType value(stmt.getColumn(0));
		if (value < strictness) {
			strictness = std::move(value);
		}
		count++;
	}

	if (count != 0 && current != strictness) {
		current = strictness;
		return true;
	}

	return false;
}

template<typename DataType, typename EnforceModel>
void GlobalPolicy<DataType, EnforceModel>::update(const std::string& admin, uid_t domain, const DataType& value)
{
	std::string selectQuery = "SELECT id FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt0 = PolicyStorage::prepare(selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(domain));
	if (!stmt0.step()) {
		throw runtime::Exception("Unknown device admin client: " + admin);
	}

	int aid = stmt0.getColumn(0);

	std::string updateQuery = "UPDATE managed_policy SET value = ? WHERE pid = ? AND aid = ?";
	DataSet stmt = PolicyStorage::prepare(updateQuery);
	stmt.bind(1, value);
	stmt.bind(2, id);
	stmt.bind(3, aid);
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to update policy");
	}
}
#endif //__DPM_POLICY_MODEL_H__
