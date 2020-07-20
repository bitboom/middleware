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

#include "policy-manager.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>

namespace vist {
namespace policy {

PolicyManager::PolicyManager() : storage(DB_PATH)
{
}

void PolicyManager::addProvider(std::shared_ptr<PolicyProvider>&& provider)
{
	for (const auto& p : this->providers) {
		if (p->getName() == provider->getName()) {
			INFO(VIST) << "Previous added provider: " << provider->getName();
			return;
		}
	}

	for (const auto& [name, policy] : provider->policies) {
		this->policies[name] = provider->getName();

		if (!storage.exists(name))
			storage.define(name, policy->getInitial());
	}

	INFO(VIST) << "Added " << provider->policies.size()
			   << "-policies from " << provider->getName();
	this->providers.emplace_back(std::move(provider));
}

void PolicyManager::enroll(const std::string& admin)
{
	this->storage.enroll(admin);
}

void PolicyManager::disenroll(const std::string& admin)
{
	this->storage.disenroll(admin);
}

void PolicyManager::activate(const std::string& admin, bool state)
{
	this->storage.activate(admin, state);
}

bool PolicyManager::isActivated()
{
	return this->storage.isActivated();
}

void PolicyManager::set(const std::string& policy,
						const PolicyValue& value,
						const std::string& admin)
{
	this->storage.update(admin, policy, value);
	this->getPolicy(policy)->set(value);
}

PolicyValue PolicyManager::get(const std::string& policy)
{
	return storage.strictest(this->getPolicy(policy));
}

std::unordered_map<std::string, PolicyValue> PolicyManager::getAll()
{
	std::unordered_map<std::string, PolicyValue> policies;
	for (const auto& pair : this->policies) {
		std::string policyName = pair.first;
		auto value = this->get(policyName);

		policies.emplace(std::move(policyName), std::move(value));
	}

	return policies;
}

std::unordered_map<std::string, int> PolicyManager::getAdmins()
{
	return storage.getAdmins();
}

const std::shared_ptr<PolicyModel>& PolicyManager::getPolicy(const std::string& name)
{
	if (this->policies.find(name) == this->policies.end())
		THROW(ErrCode::RuntimeError) << "Not exist policy: " << name;

	auto provider = this->policies[name];
	auto iter = std::find_if(this->providers.begin(), this->providers.end(),
	[&provider](const std::shared_ptr<PolicyProvider>& p) {
		return p->getName() == provider;
	});
	if (iter == this->providers.end())
		THROW(ErrCode::RuntimeError) << "Not exist provider[" << provider
									 << "] about policy: " << name;

	return (*iter)->policies[name];
}

} // namespace policy
} // namespace vist
