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
#include <klay/db/query-builder.h>

#include "pil/logger.h"

#include "sql-backend.h"
#include "db-schema.h"

using namespace query_builder;
using namespace schema;

namespace {

auto admin = make_table("admin",
						make_column("id", &Admin::id),
						make_column("pkg", &Admin::pkg),
						make_column("uid", &Admin::uid),
						make_column("key", &Admin::key),
						make_column("removable", &Admin::removable));

auto managedPolicy = make_table("managed_policy",
								 make_column("id", &ManagedPolicy::id),
								 make_column("aid", &ManagedPolicy::aid),
								 make_column("pid", &ManagedPolicy::pid),
								 make_column("value", &ManagedPolicy::value));

auto policyDefinition = make_table("policy_definition",
								   make_column("id", &PolicyDefinition::id),
								   make_column("scope", &PolicyDefinition::scope),
								   make_column("name", &PolicyDefinition::name),
								   make_column("ivalue", &PolicyDefinition::ivalue));

auto dpm = make_database("dpm", admin, managedPolicy, policyDefinition);

} // anonymous namespace

int SQLBackend::open(const std::string& path)
{
	try {
		database.reset(new database::Connection(path, database::Connection::ReadWrite |
													  database::Connection::Create));
		return 0;
	} catch (runtime::Exception& e) {
		ERROR(DPM, e.what());
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
	std::string query = policyDefinition.select(&PolicyDefinition::id,
												&PolicyDefinition::ivalue)
										.where(expr(&PolicyDefinition::name) == name);
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
	std::string query = dpm.select(&ManagedPolicy::value)
						   .join<PolicyDefinition>()
						   .on(expr(&ManagedPolicy::pid) == expr(&PolicyDefinition::id))
						   .join<Admin>()
						   .on(expr(&ManagedPolicy::aid) == expr(&Admin::id))
						   .where(expr(&ManagedPolicy::pid) == id);

	if (domain) {
		query += "AND admin.uid = ? ";
	}

	database::Statement stmt(*database, query);
	stmt.bind(1, id);
	if (domain) {
		stmt.bind(2, static_cast<int>(domain));
	}

	while (stmt.step()) {
		updated = value.strictize(DataSetInt(stmt.getColumn(0)));
	}

	return updated;
}

void SQLBackend::update(int id, const std::string& name, uid_t domain, const DataSetInt& value)
{
	int uid = static_cast<int>(domain);
	std::string selectQuery = admin.select(&Admin::id).where(expr(&Admin::pkg) == name &&
															 expr(&Admin::uid) == uid);
	database::Statement stmt0(*database, selectQuery);
	stmt0.bind(1, name);
	stmt0.bind(2, uid);
	if (!stmt0.step()) {
		throw runtime::Exception("Unknown device admin client: " + name);
	}

	int aid = stmt0.getColumn(0);

	std::string updateQuery = managedPolicy.update(&ManagedPolicy::value)
										   .where(expr(&ManagedPolicy::pid) == id &&
												  expr(&ManagedPolicy::aid) == aid);
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
	int uid = static_cast<int>(domain);
	std::string selectQuery = admin.selectAll().where(expr(&Admin::pkg) == name &&
													  expr(&Admin::uid) == uid);
	database::Statement stmt0(*database, selectQuery);
	stmt0.bind(1, name);
	stmt0.bind(2, uid);
	if (stmt0.step())
		return 0;

	std::string key = "Not supported";

	std::string insertQuery = admin.insert(&Admin::pkg, &Admin::uid,
										   &Admin::key, &Admin::removable);
	database::Statement stmt(*database, insertQuery);
	stmt.bind(1, name);
	stmt.bind(2, uid);
	stmt.bind(3, key);
	stmt.bind(4, true);
	if (!stmt.exec())
		return -1;

	return 0;
}

int SQLBackend::unenroll(const std::string& name, uid_t domain)
{
	int uid = static_cast<int>(domain);
	std::string query = admin.remove().where(expr(&Admin::pkg) == name &&
											 expr(&Admin::uid) == uid);
	database::Statement stmt(*database, query);
	stmt.bind(1, name);
	stmt.bind(2, uid);
	if (!stmt.exec())
		return -1;

	return 0;
}

std::vector<uid_t> SQLBackend::fetchDomains()
{
	std::vector<uid_t> managedDomains;
	std::string query = admin.select(distinct(&Admin::uid));
	database::Statement stmt(*database, query);
	while (stmt.step()) {
		managedDomains.push_back(stmt.getColumn(0).getInt());
	}

	return managedDomains;
}
