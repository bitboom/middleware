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
#include <sys/types.h>
#include <sys/stat.h>

#include <functional>

#include <cynara-client.h>
#include <cynara-session.h>

#include "policy-event.h"

#include "server.h"
#include "sql-backend.h"

#include "exception.h"
#include "filesystem.h"
#include "logger.h"

using namespace std::placeholders;

namespace {

const std::string PolicyStoragePath = "/opt/dbspace/.dpm.db";
const std::string PolicyManagerSocket = "/tmp/.device-policy-manager.sock";
const std::string PolicyPluginBase = "/opt/data/dpm/plugins";

} // namespace

DevicePolicyManager::DevicePolicyManager() :
	rmi::Service(PolicyManagerSocket)
{
	initPolicyStorage();

	PolicyEventNotifier::setSignalBackend(this);

	setPrivilegeChecker(std::bind(&DevicePolicyManager::checkPeerPrivilege, this, _1, _2));

	expose(this, "", (runtime::FileDescriptor)(DevicePolicyManager::subscribeSignal)(std::string));

	expose(this, "", (int)(DevicePolicyManager::enroll)(std::string, uid_t));
	expose(this, "", (int)(DevicePolicyManager::disenroll)(std::string, uid_t));
}

DevicePolicyManager::~DevicePolicyManager()
{
}

void DevicePolicyManager::loadPolicyPlugins()
{
	policyLoader.reset(new PolicyLoader(PolicyPluginBase));

	runtime::DirectoryIterator iter(PolicyPluginBase), end;
	while (iter != end) {
		if (iter->isFile()) {
			AbstractPolicyProvider* instance = policyLoader->instantiate(iter->getName(), *this);
			if (instance == nullptr) {
				ERROR("Failed to instantiate");
			}
			policyList.push_back(instance);
		}
		++iter;
	}
}

void DevicePolicyManager::initPolicyStorage()
{
	SQLBackend *backend = new SQLBackend();

	if (backend->open(PolicyStoragePath)) {
		throw runtime::Exception("Policy storage I/O error");
	}

	PolicyStorage::setBackend(backend);
}

void DevicePolicyManager::run(bool activate)
{
	::umask(0);
	start();
}

runtime::FileDescriptor DevicePolicyManager::subscribeSignal(const std::string& name)
{
	return runtime::FileDescriptor(subscribeNotification(name), true);
}

int DevicePolicyManager::enroll(const std::string& name, uid_t uid)
{
	return PolicyStorage::enroll(name, uid);
}

int DevicePolicyManager::disenroll(const std::string& name, uid_t uid)
{
	return PolicyStorage::unenroll(name, uid);
}

int DevicePolicyManager::loadManagedClients()
{
	return PolicyStorage::fetchDomains().size();
}

bool DevicePolicyManager::checkPeerPrivilege(const rmi::Credentials& cred, const std::string& privilege)
{
	if (!privilege.empty()) {
		cynara *p_cynara;

		if (::cynara_initialize(&p_cynara, NULL) != CYNARA_API_SUCCESS) {
			ERROR("Failure in cynara API");
			return false;
		}

		if (::cynara_check(p_cynara, cred.security.c_str(), "",
						   std::to_string(cred.uid).c_str(),
						   privilege.c_str()) != CYNARA_API_ACCESS_ALLOWED) {
			::cynara_finish(p_cynara);
			ERROR("Access denied: " + cred.security + " : " + privilege);
			return false;
		}

		::cynara_finish(p_cynara);
	}

	return true;
}
