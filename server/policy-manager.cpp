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
#include <climits>

#include "policy-manager.h"

#include "exception.h"
#include "filesystem.h"
#include "audit/logger.h"

#define POLICY_SCOPE_GLOBAL	0
#define POLICY_SCOPE_USER	1

#define DEFINE_USER_POLICY(_n_, _v_, _c_)	\
{ _n_, ManagedPolicy(POLICY_SCOPE_USER, _v_, _c_) }

#define DEFINE_GLOBAL_POLICY(_n_, _v_, _c_)	\
{ _n_, ManagedPolicy(POLICY_SCOPE_GLOBAL, _v_, _c_) }

namespace {

struct ManagedPolicy {
	ManagedPolicy(int sc, int val, PolicyManager::PolicyComparator pred) :
		scope(sc), value(val), compare(pred)
	{
	}

	int scope;
	int value;
	PolicyManager::PolicyComparator compare;
};

bool StateComparator(int v1, int v2)
{
	return ((v1 != v2) && (v2 == 0)) ? true : false;
}

bool RestrictionComparator(int v1, int v2)
{
	return ((v1 != v2) && (v2 == 1)) ? true : false;
}

bool MinimizeIntegerComparator(int v1, int v2)
{
	return v2 < v1 ? true : false;
}

bool MaximizeIntegerComparator(int v1, int v2)
{
	return v1 < v2 ? true : false;
}

std::unordered_map<std::string, ManagedPolicy> managedPolicyMap = {
	DEFINE_GLOBAL_POLICY("password-history", 0, MaximizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-minimum-length", 0, MaximizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-minimum-complexity", 0, MaximizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-inactivity-timeout", INT_MAX, MinimizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-expired", INT_MAX, MinimizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-maximum-failure-count", INT_MAX, MinimizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-numeric-sequences-length", INT_MAX, MinimizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-maximum-character-occurrences", INT_MAX, MinimizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-quality", 0, MaximizeIntegerComparator),
	DEFINE_GLOBAL_POLICY("password-recovery", 0, RestrictionComparator),
	DEFINE_GLOBAL_POLICY("bluetooth", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("bluetooth-tethering", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("bluetooth-desktop-connectivity", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("bluetooth-pairing", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("bluetooth-uuid-restriction", 0, RestrictionComparator),
	DEFINE_GLOBAL_POLICY("bluetooth-device-restriction", 0, RestrictionComparator),
	DEFINE_GLOBAL_POLICY("browser", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("camera", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("clipboard", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("external-storage", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("location", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("messaging", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("microphone", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("popimap-email", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("usb-debugging", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("usb-tethering", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("wifi", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("wifi-hotspot", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("wifi-profile-change", 1, StateComparator),
	DEFINE_GLOBAL_POLICY("wifi-ssid-restriction", 0, RestrictionComparator)
};

} // namespace

PolicyManager::PolicyManager(const std::string& base, const std::string& path) :
	store(base), location(path)
{
	runtime::File policyDirectory(location);

	if (policyDirectory.exists()) {
		policyDirectory.remove(true);
	}

	policyDirectory.makeDirectory(true);

	runtime::File policyStore(store);
	if (!policyStore.exists()) {
		policyStore.makeDirectory(true);
	}
}

PolicyManager::~PolicyManager()
{
}

int PolicyManager::getEffectivePolicy(const std::string& path)
{
	int value;
	runtime::File file(path);

	file.open(O_RDONLY);
	file.read(&value, sizeof(value));
	file.close();

	return value;
}

void PolicyManager::setEffectivePolicy(const std::string& path, int value)
{
	runtime::File file(path);

	file.open(O_WRONLY | O_TRUNC);
	file.write(&value, sizeof(value));
	file.close();
}

void PolicyManager::createEffectivePolicy(const std::string& path, int value)
{
	runtime::File file(path);

	file.create(0644);
	file.write(&value, sizeof(value));
	file.close();
}

void PolicyManager::prepareGlobalPolicy()
{
	for (auto it = managedPolicyMap.begin(); it != managedPolicyMap.end(); ++it) {
		const ManagedPolicy& policy = it->second;
		if (policy.scope == POLICY_SCOPE_GLOBAL) {
			std::string path = buildLocation(it->first);
			createEffectivePolicy(path, policy.value);
		}
	}
}

void PolicyManager::removeGlobalPolicy()
{
	runtime::File dir(location);
	if (dir.exists()) {
		dir.remove(true);
	}
}

void PolicyManager::removeUserPolicy(uid_t uid)
{
	runtime::File dir(location + "/" + std::to_string(uid));
	if (dir.exists()) {
		dir.remove(true);
	}
}

void PolicyManager::prepareUserPolicy(uid_t user)
{
	runtime::File dir(location + "/" + std::to_string(user));
	if (!dir.exists()) {
		dir.makeDirectory(true);
		for (auto it = managedPolicyMap.begin(); it != managedPolicyMap.end(); ++it) {
			const ManagedPolicy& policy = it->second;
			if (policy.scope == POLICY_SCOPE_USER) {
				std::string path = buildLocation(user, it->first);
				createEffectivePolicy(path, policy.value);
			}
		}
	}
}

void PolicyManager::populateStorage(const std::string& name, uid_t uid, bool create)
{
	std::unique_ptr<PolicyStorage> storage(new PolicyStorage(store, name, uid, create));
	for (auto it = storage->begin(); it != storage->end(); ++it) {
		if (managedPolicyMap.count(it->first)) {
			const ManagedPolicy& policy = managedPolicyMap.at(it->first);
			std::string path = (policy.scope == POLICY_SCOPE_USER)
							 ? buildLocation(uid, it->first)
							 : buildLocation(it->first);
			try {
				int value = getEffectivePolicy(path);
				int local = it->second.getContent();

				if (policy.compare(value, local)) {
					setEffectivePolicy(path, local);
				}
			} catch (runtime::Exception& e) {
				ERROR(e.what());
			}
		}
	}

	storageList.emplace_back(std::move(storage));
}

void PolicyManager::removeStorage(const std::string& name, uid_t uid)
{
	std::vector<std::unique_ptr<PolicyStorage>>::iterator iter = storageList.begin();
	while (iter != storageList.end()) {
		if ((*iter)->isAssociated(name, uid)) {
			(*iter)->remove();
			storageList.erase(iter);
			break;
		}
		++iter;
	}

	std::unordered_map<std::string, ManagedPolicy>::iterator it = managedPolicyMap.begin();
	while (it != managedPolicyMap.end()) {
		ManagedPolicy& policy = it->second;
		int strictness = policy.value;
		if (policy.scope == POLICY_SCOPE_USER) {
			strictness = getStrictUserPolicy(it->first, uid, strictness, policy.compare);
		} else {
			strictness = getStrictGlobalPolicy(it->first, strictness, policy.compare);
		}

		std::string path = (policy.scope == POLICY_SCOPE_USER)
						 ? buildLocation(uid, it->first)
						 : buildLocation(it->first);

		setEffectivePolicy(path, strictness);
		++it;
	}
}

void PolicyManager::cleanup()
{
	runtime::File policyDirectory(location);
	if (policyDirectory.exists()) {
		policyDirectory.remove(true);
	}
}

int PolicyManager::getGlobalPolicy(const std::string& name)
{
	if (managedPolicyMap.count(name) == 0) {
		return -1;
	}

	return getEffectivePolicy(buildLocation(name));
}

int PolicyManager::getUserPolicy(const std::string& name, uid_t uid)
{
	if (managedPolicyMap.count(name) == 0) {
		return -1;
	}
	return getEffectivePolicy(buildLocation(uid, name));
}

int PolicyManager::getStrictGlobalPolicy(const std::string& name, int pivot,
										 PolicyManager::PolicyComparator& pred)
{
	int strictness = pivot;
	std::vector<std::unique_ptr<PolicyStorage>>::iterator iter = storageList.begin();
	while (iter != storageList.end()) {
		int value = (*iter)->getPolicy(name);
		if (pred(strictness, value)) {
			strictness = value;
		}
		++iter;
	}

	return strictness;
}

int PolicyManager::getStrictUserPolicy(const std::string& name, uid_t uid, int pivot,
									   PolicyManager::PolicyComparator& pred)
{
	int strictness = pivot;
	std::vector<std::unique_ptr<PolicyStorage>>::iterator iter = storageList.begin();
	while (iter != storageList.end()) {
		if ((*iter)->isAssociated(uid)) {
			int value = (*iter)->getPolicy(name);
			if (pred(strictness, value)) {
				strictness = value;
			}
		}
		++iter;
	}

	return strictness;
}

bool PolicyManager::updateClientStorage(const std::string& pkgid, uid_t uid,
										const std::string& name, int value)
{
	std::vector<std::unique_ptr<PolicyStorage>>::iterator iter = storageList.begin();
	while (iter != storageList.end()) {
		if ((*iter)->isAssociated(pkgid, uid)) {
			(*iter)->setPolicy(name, value);
			return true;
		}
		++iter;
	}

	ERROR("Unknown client: " + pkgid);
	return false;
}

bool PolicyManager::setUserPolicy(const std::string& pkgid, uid_t uid,
								  const std::string& name, int value,
								  PolicyManager::PolicyComparator& pred)
{
	updateClientStorage(pkgid, uid, name, value);

	std::string path = buildLocation(uid, name);
	int current = getEffectivePolicy(path);
	if (current != value) {
		if (pred(current, value)) {
			setEffectivePolicy(path, value);
			return true;
		}

		int strictness = getStrictUserPolicy(name, uid, value, pred);
		if (current != strictness) {
			setEffectivePolicy(path, strictness);
			return true;
		}
	}

	return false;
}

bool PolicyManager::setGlobalPolicy(const std::string& pkgid, uid_t uid,
									const std::string& name, int value,
									PolicyManager::PolicyComparator& pred)
{
	updateClientStorage(pkgid, uid, name, value);

	std::string path = buildLocation(name);
	int current = getEffectivePolicy(path);
	if (current != value) {
		if (pred(current, value)) {
			setEffectivePolicy(path, value);
			return true;
		}

		int strictness = getStrictGlobalPolicy(name, value, pred);
		if (current != strictness) {
			setEffectivePolicy(path, strictness);
			return true;
		}
	}

	return false;
}

bool PolicyManager::setPolicy(const std::string& pkgid, uid_t uid,
							  const std::string& name, int value)
{
	if (managedPolicyMap.count(name) == 0) {
		ERROR("Unknown policy: " + name);
		return false;
	}

	ManagedPolicy& policy = managedPolicyMap.at(name);
	PolicyComparator& pred = policy.compare;

	bool ret;
	if (policy.scope == POLICY_SCOPE_USER) {
		ret = setUserPolicy(pkgid, uid, name, value, pred);
	} else {
		ret = setGlobalPolicy(pkgid, uid, name, value, pred);
	}

	return ret;
}
