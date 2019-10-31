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

#include "policy-manager.h"
#include "policy-loader.h"

#include <vist/common/audit/logger.h>

#include <klay/filesystem.h>

namespace vist {
namespace policy {

PolicyManager::PolicyManager() : storage(DB_PATH)
{
	loadProviders(PLUGIN_INSTALL_DIR);
	int cnt = loadPolicies();
	INFO(VIST, std::to_string(cnt) + "-policies loaded");
}

std::pair<int, int> PolicyManager::loadProviders(const std::string& path)
{
	INFO(VIST, "Load policies from :" << path);
	klay::File dir(path);
	if (!dir.exists() || !dir.isDirectory())
		throw std::invalid_argument("Plugin directory is wrong.: " + path);

	int passed = 0, failed = 0;
	klay::DirectoryIterator end;
	for (klay::DirectoryIterator iter(path); iter != end; ++iter) {
		if (!iter->isFile())
			continue;

		try {
			auto provider = PolicyLoader::load(iter->getPath());
			DEBUG(VIST, "Loaded provider: " << provider->getName());

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
			ERROR(VIST, "Failed to load: " << iter->getPath() << e.what());
			continue;
		}

		++passed;
	}

	INFO(VIST, "Loaded result >> passed: " << passed << ", failed: " << failed);
	return std::make_pair(passed, failed);
}

int PolicyManager::loadPolicies()
{
	bool changed = false;

	/// Make policy-provider map for performance
	for (const auto& provider : providers) {
		for (const auto& pair : provider->policies) {
			policies[pair.first] = provider->getName();

			/// Check the policy is defined on policy-storage
			if (!storage.exists(pair.first)) {
				INFO(VIST, "Define policy: " << pair.first);
				storage.define(pair.first, pair.second->getInitial().value);
				changed = true;
			}
		}
	}

	if (changed)
		storage.syncPolicyDefinition();

	return policies.size();
}

void PolicyManager::enroll(const std::string& admin)
{
	this->storage.enroll(admin);
}

void PolicyManager::disenroll(const std::string& admin)
{
	this->storage.disenroll(admin);
}

void PolicyManager::set(const std::string& policy,
						const PolicyValue& value,
						const std::string& admin)
{
	if (this->policies.find(policy) == this->policies.end())
		std::runtime_error("Not exist policy: " + policy);

	storage.update(admin, policy, value);

	for (auto& p : providers) {
		if (p->getName() != this->policies[policy])
			continue;

		if (p->policies.find(policy) != p->policies.end()) {
			p->policies[policy]->set(value);
			return;
		}
	}
}

PolicyValue PolicyManager::get(const std::string& policy)
{
	return storage.strictest(policy);
}

std::unordered_map<std::string, PolicyValue> PolicyManager::getAll()
{
	return storage.strictest();
}

std::vector<std::string> PolicyManager::getAdmins()
{
	return storage.getAdmins();
}

} // namespace policy
} // namespace vist
