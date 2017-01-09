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

#include "policy-storage.h"

std::shared_ptr<database::Connection> PolicyStorage::database;
Observerable PolicyStorage::listeners;

void PolicyStorage::open(const std::string& path)
{
	database.reset(new database::Connection(path, database::Connection::ReadWrite |
												  database::Connection::Create));
}

void PolicyStorage::close()
{
	database.reset();
}

void PolicyStorage::apply()
{
	std::vector<uid_t> domains = getManagedDomainList();
	listeners.notify(domains);
}

void PolicyStorage::addStorageEventListenr(Observer* listener)
{
	listeners.attach(listener);
}

void PolicyStorage::removeStorageEventListener(Observer* listener)
{
}

DataSet PolicyStorage::prepare(const std::string& query)
{
	return DataSet(*database, query);
}

std::vector<uid_t> PolicyStorage::getManagedDomainList()
{
	std::vector<uid_t> managedDomains;
	std::string query = "SELECT DISTINCT uid FROM admin;";
	DataSet stmt = prepare(query);
	while (stmt.step()) {
		managedDomains.push_back(stmt.getColumn(0).getInt());
	}

	return managedDomains;
}

int PolicyStorage::prepareStorage(const std::string& admin, uid_t domain)
{
	std::string selectQuery = "SELECT * FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt0 = prepare(selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(domain));
	if (stmt0.step()) {
		ERROR("Admin client already registered");
		return -1;
	}

	std::string key = "Not supported";

	std::string insertQuery = "INSERT INTO admin (pkg, uid, key, removable) VALUES (?, ?, ?, ?)";
	DataSet stmt = prepare(insertQuery);
	stmt.bind(1, admin);
	stmt.bind(2, static_cast<int>(domain));
	stmt.bind(3, key);
	stmt.bind(4, true);
	if (!stmt.exec()) {
		ERROR("I/O failure");
		return -1;
	}

	return 0;
}

int PolicyStorage::removeStorage(const std::string& admin, uid_t domain)
{
	std::string query = "DELETE FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt = prepare(query);
	stmt.bind(1, admin);
	stmt.bind(2, static_cast<int>(domain));
	if (!stmt.exec()) {
		ERROR("Unknown device admin client: " + admin);
		return -1;
	}

	listeners.notify({domain});

	return 0;
}
