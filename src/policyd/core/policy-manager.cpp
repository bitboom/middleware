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
	INFO(DPM, std::to_string(cnt) + "-policies loaded");
}

std::pair<int, int> PolicyManager::loadProviders(const std::string& path)
{
	INFO(DPM, "Load policies from :" << path);
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
			DEBUG(DPM, "Loaded provider: " << provider->getName());
			this->providers.emplace_back(std::move(provider));
		} catch (const std::exception& e) {
			++failed;
			ERROR(DPM, "Failed to load: " << iter->getPath() << e.what());
			continue;
		}

		++passed;
	}

	INFO(DPM, "Loaded result >> passed: " << passed << ", failed: " << failed);
	return std::make_pair(passed, failed);
}

int PolicyManager::loadPolicies()
{
	for (const auto& p : providers) {
		this->global.insert(p->global.cbegin(), p->global.cend());
		this->domain.insert(p->domain.cbegin(), p->domain.cend());
	}

	for (const auto& g : global) {
		if (!storage.exists(g.first))
			throw std::runtime_error("Policy does not exist.: " + g.first);
	}

	for (const auto& d : domain) {
		if (!storage.exists(d.first))
			throw std::runtime_error("Policy does not exist.: " + d.first);
	}

	return global.size() + domain.size();
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
	storage.update(admin, uid, policy, value);

	if (global.find(policy) != global.end()) {
		global[policy]->set(value);
		return;
	}

	if (domain.find(policy) != domain.end()) {
		domain[policy]->set(uid, value);
		return;
	}

	throw std::runtime_error("Cannot set policy." + policy);
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
