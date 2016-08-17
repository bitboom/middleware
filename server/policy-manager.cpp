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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "policy-manager.h"

#include "exception.h"
#include "filesystem.h"
#include "audit/logger.h"

namespace {
std::unordered_map<std::string, PolicyStateComparator> policyStateComparators;
}

PolicyManager::PolicyManager(const std::string& path) :
	location(path)
{
	runtime::File policyDirectory(location);

	if (policyDirectory.exists()) {
		if (policyDirectory.isDirectory()) {
			return;
		} else {
			policyDirectory.remove(true);
		}
	}
	policyDirectory.makeDirectory(true);
}

PolicyManager::~PolicyManager()
{
}

void PolicyManager::flushEffectivePolicy(const std::string& name)
{
	bool isFirstClient = true;
	std::string result;

	auto it = policyStateComparators.find(name);
	if (it == policyStateComparators.end()) {
		return;
	}

	PolicyStateComparator comparator = it->second;
	ClientManager& clientManager = ClientManager::instance();

	for (Client& client : clientManager.getClients()) {
		PolicyStorage& policyStorage = client.getPolicyStorage();
		const std::string& newState = policyStorage.getPolicy(name).getContent();
		if (isFirstClient) {
			isFirstClient = false;
			result = newState;
		} else if (comparator(result, newState)) {
			result = newState;
		}
	}

	runtime::File file(location + "/" + name);
	file.open(O_WRONLY | O_TRUNC);
	file.write(result.c_str(), result.size());
	file.close();
}

void PolicyManager::definePolicy(const std::string& name,
		const std::string& defaultVal, PolicyStateComparator comparator)
{
	runtime::File file(location + "/" + name);
	file.create(0640);
	file.close();

	policyStateComparators.insert(std::make_pair(name, comparator));

	flushEffectivePolicy(name);
}

const std::string PolicyManager::getPolicy(const std::string& name)
{
	runtime::File file(location + "/" + name);
	size_t size = file.size();

	std::unique_ptr<char[]> data(new char[size + 1]);
	data[size] = '\0';

	file.open(O_RDONLY);
	file.read(data.get(), size);
	file.close();

	return std::string(data.get());
}

void PolicyManager::setPolicy(Client& client, const std::string& name, const std::string& state)
{
	PolicyStorage& policyStorage = client.getPolicyStorage();
	policyStorage.getPolicy(name).setContent(state);
	policyStorage.flush();

	flushEffectivePolicy(name);
}

void PolicyManager::remove()
{
	runtime::File policyDirectory(location);
	if (policyDirectory.exists()) {
		policyDirectory.remove(true);
	}
}
