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

#include <string>
#include <climits>
#include <stdexcept>
#include <klay/exception.h>

#include "policy.h"

#include "client-manager.h"

namespace {

const std::string clientPolicyStorage = CONF_PATH "/policy";

} //namespace

Client::Client(const std::string& pkgname, uid_t puid, const std::string& pk) :
	name(pkgname), uid(puid), key(pk), policyStorage(nullptr)
{
	std::string storagePath = clientPolicyStorage + "/" + name + "-" + std::to_string(uid) + ".xml";
	policyStorage.reset(new PolicyStorage(storagePath));
}

Client::~Client()
{
}

namespace {

const std::string dataStorageLocation = DB_PATH;
const std::string clientDBName = ".client.db";

} //namespace

ClientManager::ClientManager() :
	clientRepository(nullptr)
{
	std::string location = dataStorageLocation + "/" + clientDBName;
	clientRepository.reset(new database::Connection(location, database::Connection::ReadWrite |
						   database::Connection::Create));
	prepareRepository();
	loadClients();
}

ClientManager::~ClientManager()
{
}

void ClientManager::registerClient(const std::string& name, uid_t uid)
{
	std::lock_guard<Mutex> lock(mutex);

	std::string selectQuery = "SELECT * FROM CLIENT WHERE PKG = \"" + name + "\"" + " AND UID = \"" + std::to_string(uid) + "\"";
	database::Statement stmt0(*clientRepository, selectQuery);
	if (stmt0.step()) {
		throw runtime::Exception("Client already registered");
	}

	std::string key = generateKey();

	std::string insertQuery = "INSERT INTO CLIENT (PKG, UID, KEY, VALID) VALUES (?, ?, ?, ?)";
	database::Statement stmt(*clientRepository, insertQuery);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(uid));
	stmt.bind(3, key);
	stmt.bind(4, true);

	if (!stmt.exec()) {
		throw runtime::Exception("Failed to insert client data");
	}

	registeredClients.push_back(Client(name, uid, key));
}

void ClientManager::deregisterClient(const std::string& name, uid_t uid)
{
	auto removeClient = [](ClientList & list, const std::string& name, uid_t uid) {
		ClientList::iterator iter = list.begin();
		while (iter != list.end()) {
			Client& client = *iter;
			if (client.getName() == name && client.getUid() == uid) {
				client.removePolicyStorage();
				list.erase(iter);
				return true;
			}
			++iter;
		}

		return false;
	};

	std::lock_guard<Mutex> lock(mutex);

	std::string query = "DELETE FROM CLIENT WHERE PKG = \"" + name + "\"" + " AND UID = \"" + std::to_string(uid) + "\"";
	if (!clientRepository->exec(query)) {
		throw runtime::Exception("Failed to delete client data");
	}

	if (!removeClient(registeredClients, name, uid)) {
		removeClient(activatedClients, name, uid);
	}
}

Client& ClientManager::getClient(const std::string& name, uid_t uid)
{
	for (Client& client : getClients()) {
		if (client.getName() == name && client.getUid() == uid) {
			return client;
		}
	}
	throw runtime::Exception("Client doesn't exist");
}

std::string ClientManager::generateKey()
{
	std::string key = "TestKey";
	return key;
}

void ClientManager::loadClients()
{
	std::lock_guard<Mutex> lock(mutex);

	database::Statement stmt(*clientRepository, "SELECT * FROM CLIENT");
	while (stmt.step()) {
		std::string name = stmt.getColumn(1).getText();
		uid_t uid = static_cast<uid_t>(stmt.getColumn(2).getInt());
		std::string key = stmt.getColumn(3).getText();

		registeredClients.push_back(Client(name, uid, key));
	}
	registeredClients.push_back(Client("org.tizen.dpm-toolkit", 5001, "TestKey"));
}

void ClientManager::prepareRepository()
{
	std::string query = "CREATE TABLE IF NOT EXISTS CLIENT ("    \
						"ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
						"PKG TEXT, "                             \
						"UID INTEGER, "                          \
						"KEY TEXT, "                             \
						"VALID INTEGER)";

	clientRepository->exec(query);
}

ClientManager& ClientManager::instance()
{
	static ClientManager __instance__;
	return __instance__;
}
