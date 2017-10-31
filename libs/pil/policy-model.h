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
#include <iostream>

#include <klay/exception.h>

#include "policy-storage.h"
#include "policy-admin.h"
#include "observer.h"

class AbstractPolicyProvider {
public:
	AbstractPolicyProvider() {}
	virtual ~AbstractPolicyProvider() {}
};

template<typename Type>
class DomainPolicy : public Observer {
public:
	typedef Type DataType;

	DomainPolicy(const std::string& name);
	virtual ~DomainPolicy();

	void set(const Type& value);
	DataType get();

	void onEvent(uid_t domain);

	virtual bool apply(const Type& value, uid_t domain) {}

private:
	int id;
	Type initial;
	std::unordered_map<uid_t, Type> current;
};

template<typename Type>
DomainPolicy<Type>::DomainPolicy(const std::string& name) :
	id(-1), current()
{
	id = PolicyStorage::define(name, initial);
	if (id < 0) {
		throw runtime::Exception("Failed to define policy");
	}

	std::vector<uid_t> domains = PolicyStorage::fetchDomains();
	for (auto domid : domains) {
		current[domid] = initial;
		PolicyStorage::strictize(id, current[domid], domid);
	}

	PolicyStorage::subscribeEvent(this);
}

template<typename Type>
DomainPolicy<Type>::~DomainPolicy()
{
}

template<typename Type>
void DomainPolicy<Type>::set(const DataType& val)
{
	PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());

	PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

	Type value = initial;
	PolicyStorage::strictize(id, value, admin.getUid());

	if (current[admin.getUid()] != value) {
		apply(value, admin.getUid());
		current[admin.getUid()] = value;
	}
}

template<typename Type>
Type DomainPolicy<Type>::get()
{
	uid_t domain = rmi::Service::getPeerUid();
	if (!current.count(domain))
		return initial;

	return current[domain].value;
}

template<typename Type>
void DomainPolicy<Type>::onEvent(uid_t domain)
{
	Type value = initial;
	PolicyStorage::strictize(id, value, domain);
	if (current[domain] != value) {
		apply(value, domain);
		current[domain] = value;
	}
}

template<typename Type>
class GlobalPolicy : public Observer {
public:
	typedef Type DataType;

	GlobalPolicy(const std::string& name);
	virtual ~GlobalPolicy();

	void set(const Type& value);
	Type get();

	void onEvent(uid_t domain);

	virtual bool apply(const Type& value) {}

private:
	int id;
	Type initial;
	Type current;
};

template<typename Type>
GlobalPolicy<Type>::GlobalPolicy(const std::string& name) :
	id(-1), current()
{
	id = PolicyStorage::define(name, initial);
	current = initial;
	PolicyStorage::strictize(id, current);
	PolicyStorage::subscribeEvent(this);
}

template<typename Type>
GlobalPolicy<Type>::~GlobalPolicy()
{
}

template<typename Type>
void GlobalPolicy<Type>::set(const Type& val)
{
	PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());

	PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

	Type value = initial;
	PolicyStorage::strictize(id, value);
	if (current != value) {
		apply(value);
		current = value;
	}
}

template<typename Type>
Type GlobalPolicy<Type>::get()
{
	return current.value;
}

template<typename Type>
void GlobalPolicy<Type>::onEvent(uid_t domain)
{
	Type value = initial;
	PolicyStorage::strictize(id, value);
	if (current != value) {
		apply(value);
		current = value;
	}
}
#endif //__DPM_POLICY_MODEL_H__
