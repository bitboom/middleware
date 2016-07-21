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

#ifndef __DPM_CLIENT_MANAGER_H__
#define __DPM_CLIENT_MANAGER_H__

#include <memory>
#include <stdexcept>
#include <string>
#include <mutex>

#include <klay/db/column.h>
#include <klay/db/statement.h>
#include <klay/db/connection.h>

#include "policy.h"
#include "policy-storage.h"

class Client {
public:
	Client(const Client&) = delete;
	Client(Client&&) = default;
	Client(const std::string& name, uid_t uid, const std::string& key);

	~Client();

	Client& operator=(Client&&) = default;
	Client& operator=(const Client&) = delete;

	inline std::string getName() const
	{
		return name;
	}

	inline uid_t getUid() const
	{
		return uid;
	}

	inline std::string getKey() const
	{
		return key;
	}

	PolicyStorage& getPolicyStorage() {
		return *policyStorage.get();
	}

	void removePolicyStorage() {
		policyStorage.get()->remove();
	}

private:
	std::string name;
	uid_t uid;
	std::string key;

	std::unique_ptr<PolicyStorage> policyStorage;
};

class ClientManager {
public:
	typedef std::vector<Client> ClientList;

	ClientManager(const ClientManager&) = delete;
	ClientManager& operator=(const ClientManager&) = delete;

	void registerClient(const std::string& name, uid_t uid);
	void deregisterClient(const std::string& name, uid_t uid);
	Client& getClient(const std::string& name, uid_t uid);

	ClientList& getClients() {
		return registeredClients;
	}

	std::string generateKey();

	static ClientManager& instance();

private:
	ClientManager();
	~ClientManager();

	void loadClients();
	void prepareRepository();
	std::string getPackageName(int pid);

private:
	ClientList activatedClients;
	ClientList registeredClients;

	std::unique_ptr<database::Connection> clientRepository;

	typedef std::recursive_mutex Mutex;
	Mutex mutex;
};

#endif //__DPM_CLIENT_MANAGER_H__
