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
#include "policy-loader.hpp"

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>

namespace vist {
namespace policy {

PolicyManager::PolicyManager() : storage(DB_PATH)
{
	loadProviders(PLUGIN_INSTALL_DIR);
	int cnt = loadPolicies();
	INFO(VIST) << std::to_string(cnt) << "-policies loaded";

	this->storage.enroll(DEFAULT_ADMIN_PATH);
}

std::pair<int, int> PolicyManager::loadProviders(const std::string& path)
{
	INFO(VIST) << "Load policies from :" << path;
	using namespace boost::filesystem;
	if (!is_directory(path))
		THROW(ErrCode::LogicError) << "Plugin directory is wrong.: " << path;

	int passed = 0, failed = 0;
	for (directory_entry& entry : directory_iterator(path)) {
		if (!is_regular_file(entry.path().string()))
			continue;

		try {
			auto provider = PolicyLoader::load(entry.path().string());
			DEBUG(VIST) << "Loaded provider: " << provider->getName();

			bool exist = false;
			for (const auto& p : this->providers) {
				if (p->getName() == provider->getName()) {
					exist = true;
					break;
				}
			}

			if (!exist)
				this->providers.emplace_back(std::move(provider));
		} catch (const std::exception& e) {
			++failed;
			ERROR(VIST) << "Failed to load: " << entry.path().string() << e.what();
			continue;
		}

		++passed;
	}

	INFO(VIST) << "Loaded result >> passed: " << passed << ", failed: " << failed;
	return std::make_pair(passed, failed);
}

int PolicyManager::loadPolicies()
{
	/// Make policy-provider map for performance
	for (const auto& provider : providers) {
		for (const auto& pair : provider->policies) {
			std::string policy = pair.first;
			this->policies[policy] = provider->getName();

			/// Check the policy is defined on policy-storage
			if (!storage.exists(pair.first)) {
				storage.define(pair.first, pair.second->getInitial());
			}
		}
	}

	return this->policies.size();
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
							 [&provider](const std::unique_ptr<PolicyProvider>& p) {
								 return p->getName() == provider;
							 });
	if (iter == this->providers.end())
		THROW(ErrCode::RuntimeError) << "Not exist provider[" << provider
									 << "] about policy: " << name;

	return (*iter)->policies[name];
}

} // namespace policy
} // namespace vist
