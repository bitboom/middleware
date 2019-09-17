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

#ifndef __DEVICE_POLICY_MANAGER_H__
#define __DEVICE_POLICY_MANAGER_H__

#include <string>
#include <memory>
#include <vector>
#include <utility>

#include <vconf.h>

#include <klay/filesystem.h>
#include <klay/file-descriptor.h>
#include <klay/rmi/service.h>

#include "plugin.h"
#include "sql-backend.h"

#define MAX_CLIENT_CONNECTIONS	10

class DevicePolicyManager : public rmi::Service {
public:
	DevicePolicyManager();
	~DevicePolicyManager();

	int loadManagedClients();
	std::pair<int, int> loadPolicyPlugins();
	void applyPolicies();

	void run(int activation, int timeout);

private:
	void initPolicyStorage();

	int enroll(const std::string& name, uid_t uid);
	int disenroll(const std::string& name, uid_t uid);

	bool checkPeerPrivilege(const rmi::Credentials& cred, const std::string& privilege);
	bool checkNewConnection(const rmi::Connection& connection);
	bool checkCloseConnection(const rmi::Connection& connection);

	bool getMaintenanceMode();

private:
	typedef std::unordered_map<int, int> ClientRegistry;
	ClientRegistry clientRegistry;

	std::vector<AbstractPolicyProvider *> policyList;
	std::unique_ptr<PolicyLoader> policyLoader;
	std::thread policyApplyThread;

	runtime::File bootCompletedFile;
};

#endif //__DEVICE_POLICY_MANAGER_H__
