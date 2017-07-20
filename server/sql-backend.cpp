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

#include <klay/exception.h>

#include "sql-backend.h"

int SQLBackend::open(const std::string& path)
{
	try {
		database.reset(new database::Connection(path, database::Connection::ReadWrite |
													  database::Connection::Create));
		return 0;
	} catch (runtime::Exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return -1;
}

void SQLBackend::close()
{
	database.reset();
}

int SQLBackend::define(const std::string& name, DataSetInt& value)
{
	int id = -1;
	std::string query = "SELECT id, ivalue FROM policy_definition WHERE name = ?";
	database::Statement stmt(*database, query);

	stmt.bind(1, name);
	if (stmt.step()) {
		id = stmt.getColumn(0);
		value = stmt.getColumn(1);
	}

	return id;
}

bool SQLBackend::strictize(int id, DataSetInt& value, uid_t domain)
{
	bool updated = false;
	std::string query = "SELECT managed_policy.value FROM managed_policy " \
						"INNER JOIN policy_definition ON managed_policy.pid = policy_definition.id " \
						"INNER JOIN admin ON managed_policy.aid = admin.id " \
						"WHERE managed_policy.pid = ? ";

	if (domain) {
		query += "AND admin.uid = ? ";
	}

	database::Statement stmt(*database, query);
	stmt.bind(1, id);
	if (domain) {
		stmt.bind(2, static_cast<int>(domain));
	}

	while (stmt.step()) {
		std::cout << "Update strict value" << std::endl;
		updated = value.strictize(DataSetInt(stmt.getColumn(0)));
	}

	return updated;
}

void SQLBackend::update(int id, const std::string& admin, uid_t domain, const DataSetInt& value)
{
	std::string selectQuery = "SELECT id FROM admin WHERE pkg = ? AND uid = ?";
	database::Statement stmt0(*database, selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(domain));
	if (!stmt0.step()) {
		throw runtime::Exception("Unknown device admin client: " + admin);
	}

	int aid = stmt0.getColumn(0);

	std::string updateQuery = "UPDATE managed_policy SET value = ? WHERE pid = ? AND aid = ?";
	database::Statement stmt(*database, updateQuery);
	stmt.bind(1, value);
	stmt.bind(2, id);
	stmt.bind(3, aid);
	if (!stmt.exec()) {
		throw runtime::Exception("Failed to update policy");
	}
}

int SQLBackend::enroll(const std::string& name, uid_t domain)
{
    std::string selectQuery = "SELECT * FROM admin WHERE pkg = ? AND uid = ?";
    database::Statement stmt0(*database, selectQuery);
    stmt0.bind(1, name);
    stmt0.bind(2, static_cast<int>(domain));
    if (stmt0.step())
		return -1;

    std::string key = "Not supported";

    std::string insertQuery = "INSERT INTO admin (pkg, uid, key, removable) VALUES (?, ?, ?, ?)";
    database::Statement stmt(*database, insertQuery);
    stmt.bind(1, name);
    stmt.bind(2, static_cast<int>(domain));
    stmt.bind(3, key);
    stmt.bind(4, true);
    if (!stmt.exec())
		return -1;

	return 0;
}

int SQLBackend::unenroll(const std::string& name, uid_t domain)
{
	std::string query = "DELETE FROM admin WHERE pkg = ? AND uid = ?";
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(domain));
	if (!stmt.exec())
		return -1;

	return 0;
}

std::vector<uid_t> SQLBackend::fetchDomains()
{
	std::vector<uid_t> managedDomains;
	std::string query = "SELECT DISTINCT uid FROM admin;";
	database::Statement stmt(*database, query);
	while (stmt.step()) {
		managedDomains.push_back(stmt.getColumn(0).getInt());
	}

	return managedDomains;
}
