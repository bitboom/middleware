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

#include <klay/exception.h>

#include "client-manager.h"

DeviceAdministrator::DeviceAdministrator(const std::string& pkgname, uid_t puid, const std::string& pk) :
	name(pkgname), uid(puid), key(pk)
{
}

DeviceAdministrator::~DeviceAdministrator()
{
}

DeviceAdministratorManager::DeviceAdministratorManager(const std::string& path) :
	repository(path + "/.dpm.db")
{
	prepareRepository();
}

DeviceAdministrator DeviceAdministratorManager::enroll(const std::string& name, uid_t uid)
{
	database::Connection connection(repository, database::Connection::ReadWrite);

	std::string selectQuery = "SELECT * FROM ADMIN WHERE PKG = ? AND UID = ?";
	database::Statement stmt0(connection, selectQuery);
	stmt0.bind(1, name);
	stmt0.bind(2, static_cast<int>(uid));
	if (stmt0.step()) {
		throw runtime::Exception("Client already registered");
	}

	std::string key = generateKey();

	std::string insertQuery = "INSERT INTO ADMIN (PKG, UID, KEY, REMOVABLE) VALUES (?, ?, ?, ?)";
	database::Statement stmt(connection, insertQuery);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(uid));
	stmt.bind(3, key);
	stmt.bind(4, true);
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to insert client data");
	}

	return DeviceAdministrator(name, uid, key);
}

void DeviceAdministratorManager::disenroll(const std::string& name, uid_t uid)
{
	database::Connection connection(repository, database::Connection::ReadWrite);

	std::string query = "DELETE FROM ADMIN WHERE PKG = ? AND UID = ?";
	database::Statement stmt(connection, query);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(uid));
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to delete client data");
	}
}

std::string DeviceAdministratorManager::generateKey()
{
	std::string key = "TestKey";
	return key;
}

void DeviceAdministratorManager::prepareRepository()
{
	database::Connection connection(repository, database::Connection::ReadWrite |
												database::Connection::Create);

	std::string query = "CREATE TABLE IF NOT EXISTS ADMIN ("    \
						"ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
						"PKG TEXT, "                             \
						"UID INTEGER, "                          \
						"KEY TEXT, "                             \
						"REMOVABLE INTEGER)";

	connection.exec(query);

	database::Statement stmt(connection, "SELECT * FROM ADMIN");
	while (stmt.step()) {
		std::string name = stmt.getColumn(1).getText();
		uid_t uid = static_cast<uid_t>(stmt.getColumn(2).getInt());
		std::string key = stmt.getColumn(3).getText();

		deviceAdministratorList.push_back(DeviceAdministrator(name, uid, key));
	}
}
