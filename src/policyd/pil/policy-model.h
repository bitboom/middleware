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
#include <vector>
#include <mutex>
#include <memory>

#include <klay/exception.h>

#include "policy-storage.h"
#include "policy-admin.h"
#include "observer.h"

class AbstractPolicy;

struct AbstractPolicyProvider {
	AbstractPolicyProvider() {}
	virtual ~AbstractPolicyProvider() {}

	std::vector<std::shared_ptr<AbstractPolicy>> policies;
};

class AbstractPolicy : public Observer {
public:
	explicit AbstractPolicy(const std::string& name) : name(name)
	{
		this->id = PolicyStorage::define(name, initial);
		if (id < 0)
			throw runtime::Exception("Failed to define policy");

		PolicyStorage::subscribeEvent(this);
	}

	~AbstractPolicy() = default;

	AbstractPolicy(const AbstractPolicy&) = delete;
	AbstractPolicy& operator=(const AbstractPolicy&) = delete;

	AbstractPolicy(AbstractPolicy&&) = default;
	AbstractPolicy& operator=(AbstractPolicy&&) = default;

	virtual void set(const DataSetInt& value) = 0;
	virtual DataSetInt get() = 0;

	virtual bool apply(const DataSetInt& value, uid_t domain = 0) = 0;

	inline void onEvent(uid_t domain)
	{
		std::lock_guard<std::mutex> mtxGuard(mtx);
		enforce(domain);
	}

	std::string name;

protected:
	int id = -1;
	std::mutex mtx;
	DataSetInt initial;

private:
	virtual void enforce(uid_t domain = 0) = 0;
};
