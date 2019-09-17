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

class GlobalPolicy : public AbstractPolicy {
public:
	GlobalPolicy(const std::string& name) : AbstractPolicy(name) {}
	virtual ~GlobalPolicy() = default;

	GlobalPolicy(const GlobalPolicy&) = delete;
	GlobalPolicy& operator=(const GlobalPolicy&) = delete;

	GlobalPolicy(GlobalPolicy&&) = default;
	GlobalPolicy& operator=(GlobalPolicy&&) = default;

	inline void set(const DataSetInt& val) override
	{
		PolicyAdmin admin(rmi::Service::getPeerPid(), rmi::Service::getPeerUid());
		PolicyStorage::update(id, admin.getName(), admin.getUid(), val);

		std::lock_guard<std::mutex> mtxGuard(mtx);
		enforce(-1);
	}

	inline DataSetInt get() override
	{
		std::lock_guard<std::mutex> mtxGuard(mtx);
		if (!ready) {
			enforce(-1);
			ready = true;
		}
		return current.value;
	}

private:
	inline void enforce(uid_t) override
	{
		auto value = initial;
		PolicyStorage::strictize(id, value);
		if (current != value) {
			apply(value);
			current = value;
		}
	}

private:
	DataSetInt current;
	bool ready = false;
};
