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

#pragma once

#include <string>
#include <unordered_map>

#include <klay/rmi/service.h>

#include "policy-model.h"

class DomainPolicy : public AbstractPolicy {
public:
	DomainPolicy(const std::string& name) : AbstractPolicy(name) {}
	virtual ~DomainPolicy() = default;

	DomainPolicy(const DomainPolicy&) = delete;
	DomainPolicy& operator=(const DomainPolicy&) = delete;

	DomainPolicy(DomainPolicy&&) = default;
	DomainPolicy& operator=(DomainPolicy&&) = default;

	inline void set(const DataSetInt& val) override
	{
		PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());
		PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

		std::lock_guard<std::mutex> mtxGuard(mtx);
		this->enforce(admin.getUid());
	}

	inline DataSetInt get() override
	{
		uid_t domain = rmi::Service::getPeerUid();

		std::lock_guard<std::mutex> mtxGuard(mtx);
		if (!current.count(domain))
			enforce(domain);

		return current[domain].value;
	}

private:
	inline void enforce(uid_t domain) override
	{
		auto value = initial;
		if (!ready[domain])
			current[domain] = initial;

		PolicyStorage::strictize(id, value, domain);
		if (current[domain] != value) {
			apply(value, domain);
			current[domain] = value;
		}

		ready[domain] = true;
	}

private:
	std::unordered_map<uid_t, DataSetInt> current;
	std::unordered_map<uid_t, bool> ready;
};
