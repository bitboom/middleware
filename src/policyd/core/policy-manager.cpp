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
		} catch (const std::exception&) {
			++failed;
			ERROR(DPM, "Failed to load: " << iter->getPath());
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

} // namespace policyd
