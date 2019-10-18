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
#include "logger.h"

#include <klay/filesystem.h>

namespace policyd {

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
		for (const auto& pair : provider->global) {
			policies[pair.first] = provider->getName();

			/// Check the policy is defined on policy-storage
			if (!storage.exists(pair.first)) {
				INFO(VIST, "Define global policy: " << pair.first);
				storage.define(0, pair.first, pair.second->getInitial().value);
				changed = true;
			}
		}

		for (const auto& pair : provider->domain) {
			policies[pair.first] = provider->getName();

			if (!storage.exists(pair.first)) {
				INFO(VIST, "Define domain policy: " << pair.first);
				storage.define(1, pair.first, pair.second->getInitial().value);
				changed = true;
			}
		}
	}

	if (changed)
		storage.syncPolicyDefinition();

	return policies.size();
}

void PolicyManager::enroll(const std::string& admin, uid_t uid)
{
	this->storage.enroll(admin, uid);
}

void PolicyManager::disenroll(const std::string& admin, uid_t uid)
{
	this->storage.disenroll(admin, uid);
}

void PolicyManager::set(const std::string& policy, const PolicyValue& value,
						const std::string& admin, uid_t uid)
{
	if (policies.find(policy) == policies.end())
		std::runtime_error("Not exist policy: " + policy);

	storage.update(admin, uid, policy, value);

	for (auto& p : providers) {
		if (p->getName() != policies[policy])
			continue;

		if (p->global.find(policy) != p->global.end()) {
			p->global[policy]->set(value);
			return;
		}

		if (p->domain.find(policy) != p->domain.end()) {
			p->domain[policy]->set(uid, value);
			return;
		}
	}
}

PolicyValue PolicyManager::get(const std::string& policy, uid_t uid)
{
	return storage.strictest(policy, uid);
}

std::unordered_map<std::string, PolicyValue> PolicyManager::getAll(uid_t uid)
{
	return storage.strictest(uid);
}

} // namespace policyd
