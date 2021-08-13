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
#include <mutex>

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

	virtual bool apply(const Type& value, uid_t domain)
	{
		return true;
	}

private:
	void enforce(uid_t domid);

private:
	int id;
	Type initial;
	std::unordered_map<uid_t, Type> current;
	std::unordered_map<uid_t, bool> ready;
	std::mutex mtx;
};

template<typename Type>
DomainPolicy<Type>::DomainPolicy(const std::string& name) :
	id(-1), current()
{
	id = PolicyStorage::define(name, initial);
	if (id < 0) {
		throw runtime::Exception("Failed to define policy");
	}

	PolicyStorage::subscribeEvent(this);
}

template<typename Type>
DomainPolicy<Type>::~DomainPolicy()
{
}

template<typename Type>
void DomainPolicy<Type>::enforce(uid_t domid)
{
	Type value = initial;
	if (!ready[domid])
		current[domid] = initial;

	PolicyStorage::strictize(id, value, domid);
	if (current[domid] != value) {
		apply(value, domid);
		current[domid] = value;
	}

	ready[domid] = true;
}

template<typename Type>
void DomainPolicy<Type>::set(const DataType& val)
{
	PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());
	PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

	std::lock_guard<std::mutex> mtxGuard(mtx);
	enforce(admin.getUid());
}

template<typename Type>
Type DomainPolicy<Type>::get()
{
	uid_t domain = rmi::Service::getPeerUid();

	std::lock_guard<std::mutex> mtxGuard(mtx);
	if (!current.count(domain)) {
		enforce(domain);
	}

	return current[domain].value;
}

template<typename Type>
void DomainPolicy<Type>::onEvent(uid_t domain)
{
	std::lock_guard<std::mutex> mtxGuard(mtx);
	enforce(domain);
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

	virtual bool apply(const Type& value)
	{
		return true;
	}

private:
	void enforce();

private:
	int id;
	Type initial;
	Type current;
	bool ready;
	std::mutex mtx;
};

template<typename Type>
GlobalPolicy<Type>::GlobalPolicy(const std::string& name) :
	id(-1), current(), ready(false)
{
	id = PolicyStorage::define(name, initial);
	if (id < 0) {
		throw runtime::Exception("Failed to define policy");
	}

	current = initial;
	PolicyStorage::subscribeEvent(this);
}

template<typename Type>
GlobalPolicy<Type>::~GlobalPolicy()
{
}

template<typename Type>
void GlobalPolicy<Type>::enforce()
{
	Type value = initial;
	PolicyStorage::strictize(id, value);
	if (current != value) {
		apply(value);
		current = value;
	}
}

template<typename Type>
void GlobalPolicy<Type>::set(const Type& val)
{
	PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());
	PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

	std::lock_guard<std::mutex> mtxGuard(mtx);
	enforce();
}

template<typename Type>
Type GlobalPolicy<Type>::get()
{
	std::lock_guard<std::mutex> mtxGuard(mtx);
	if (!ready) {
		enforce();
		ready = true;
	}
	return current.value;
}

template<typename Type>
void GlobalPolicy<Type>::onEvent(uid_t domain)
{
	std::lock_guard<std::mutex> mtxGuard(mtx);
	enforce();
}
#endif //__DPM_POLICY_MODEL_H__
