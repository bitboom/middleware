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

#include <klay/exception.h>
#include <klay/filesystem.h>

#include "pil/policy-event.h"
#include "pil/logger.h"

#include "server.h"
#include "sql-backend.h"

#include <vconf.h>

#define VCONFKEY_DPM_MODE_STATE "db/dpm/mode_state"

using namespace std::placeholders;

namespace {
const std::string PolicyStoragePath = "/opt/dbspace/.dpm.db";
const std::string PolicyManagerSocket = "/tmp/.device-policy-manager.sock";
const std::string PolicyPluginBase = PLUGIN_INSTALL_DIR;
const std::string BootCompleted = "/tmp/.dpm-bootCompleted";
const std::string BootString = "bootCompleted";

} // namespace

DevicePolicyManager::DevicePolicyManager() :
	rmi::Service(PolicyManagerSocket)
{
	initPolicyStorage();

	PolicyEventNotifier::init();

	setPrivilegeChecker(std::bind(&DevicePolicyManager::checkPeerPrivilege, this, _1, _2));
	setNewConnectionCallback(std::bind(&DevicePolicyManager::checkNewConnection, this, _1));
	setCloseConnectionCallback(std::bind(&DevicePolicyManager::checkCloseConnection, this, _1));

	expose(this, "", (int)(DevicePolicyManager::enroll)(std::string, uid_t));
	expose(this, "", (int)(DevicePolicyManager::disenroll)(std::string, uid_t));

	bootCompletedFile = runtime::File(BootCompleted);
	bootCompletedFile.create(0644);
	bootCompletedFile.close();
}

DevicePolicyManager::~DevicePolicyManager()
{
	if (policyApplyThread.joinable())
		policyApplyThread.join();

	try {
		if (bootCompletedFile.exists()) {
			bootCompletedFile.remove();
		}
	} catch (std::exception &e) {
		ERROR(DPM, e.what());
	}
}

std::pair<int, int> DevicePolicyManager::loadPolicyPlugins()
{
    runtime::File folder(PolicyPluginBase);
	if (!folder.exists() || !folder.isDirectory())
		throw runtime::Exception("No exist plugin directories.");

	policyLoader.reset(new PolicyLoader(PolicyPluginBase));

	int passed = 0, failed = 0;
	runtime::DirectoryIterator iter(PolicyPluginBase), end;
	while (iter != end) {
		if (!iter->isFile()) {
			++iter;
			continue;
		}

		auto instance = policyLoader->instantiate(iter->getName(), *this);
		if (instance == nullptr) {
			ERROR(DPM, "Failed to instantiate.: " << iter->getName());
			++failed;
			++iter;
			continue;
		}

		policyList.push_back(instance);
		++passed;
		++iter;
	}
	DEBUG(DPM, "Finished load policy-plugins: " <<
			   "passed: " << passed << ", " <<
			   "failed: " << failed);

	return std::make_pair(passed, failed);
}

void DevicePolicyManager::initPolicyStorage()
{
	SQLBackend *backend = new SQLBackend();

	if (backend->open(PolicyStoragePath)) {
		throw runtime::Exception("Policy storage I/O error");
	}

	PolicyStorage::setBackend(backend);
	DEBUG(DPM, "Success to init policy-storage.");

	int mode = 0;
	if(loadManagedClients() > 0) {
		mode = 1;
	}

	if(::vconf_set_bool(VCONFKEY_DPM_MODE_STATE, mode) != 0) {
		DEBUG(DPM, "VCONFKEY_DPM_MODE_STATE set  failed.");
	}
}

void DevicePolicyManager::applyPolicies()
{
	policyApplyThread = std::thread(
		[this]() {
			PolicyStorage::notify();

			if (bootCompletedFile.exists()) {
				bootCompletedFile.open(O_WRONLY);
				bootCompletedFile.lock();
				bootCompletedFile.write(BootString.c_str(), BootString.length());
				bootCompletedFile.unlock();
				bootCompletedFile.close();
			}
		});
}

bool DevicePolicyManager::getMaintenanceMode()
{
	return loadManagedClients() > 0 ? true : false;
}

void DevicePolicyManager::run(int activation, int timeout)
{
	do {
		start(activation, timeout);
		timeout = -1;
	} while (getMaintenanceMode());
}

int DevicePolicyManager::enroll(const std::string& name, uid_t uid)
{
	int ret = PolicyStorage::enroll(name, uid);
	if(ret == 0) {
		if(::vconf_set_bool(VCONFKEY_DPM_MODE_STATE, 1) != 0) {
			DEBUG(DPM, "VCONFKEY_DPM_MODE_STATE set  failed.");
		}
	}
	return ret;
}

int DevicePolicyManager::disenroll(const std::string& name, uid_t uid)
{
	int ret = PolicyStorage::unenroll(name, uid);
	if(ret == 0) {
		if(loadManagedClients() == 0) {
			if(::vconf_set_bool(VCONFKEY_DPM_MODE_STATE, 0) != 0) {
				DEBUG(DPM, "VCONFKEY_DPM_MODE_STATE set  failed.");
			}
			stop();
		}
	}
	return ret;
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
			ERROR(DPM, "Failure in cynara API");
			return false;
		}

		if (::cynara_check(p_cynara, cred.security.c_str(), "",
						   std::to_string(cred.uid).c_str(),
						   privilege.c_str()) != CYNARA_API_ACCESS_ALLOWED) {
			::cynara_finish(p_cynara);
			ERROR(DPM, "Access denied: " + cred.security + " : " + privilege);
			return false;
		}

		::cynara_finish(p_cynara);
	}

	return true;
}

bool DevicePolicyManager::checkNewConnection(const rmi::Connection& connection)
{
	auto pid = (connection.getPeerCredentials()).pid;
	if(clientRegistry.count(pid)) {
		if(clientRegistry[pid] >= MAX_CLIENT_CONNECTIONS) {
			return false;
		} else {
			clientRegistry[pid]++;
		}
	} else {
		clientRegistry.emplace(pid, 1);
	}

	return true;
}

bool DevicePolicyManager::checkCloseConnection(const rmi::Connection& connection)
{
	auto pid = (connection.getPeerCredentials()).pid;
	if(clientRegistry.count(pid)) {
		clientRegistry[pid]--;
		if(clientRegistry[pid] == 0) {
			clientRegistry.erase(pid);
		}
	}

	return true;
}
