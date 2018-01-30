/*
 *  Copyright (c) 2018 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "db-schema.h"

#include <klay/testbench.h>

#include <klay/db/query-builder.h>

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

TESTCASE(DEFINE)
{
	std::string name;
	std::string query = policyDefinition.select(&PolicyDefinition::id,
												&PolicyDefinition::ivalue)
										.where(expr(&PolicyDefinition::name) == name);

	std::string expect = "SELECT id, ivalue FROM policy_definition WHERE name = ?";

	TEST_EXPECT(expect, query);
}

TESTCASE(STRICTIZE)
{
	int id = 0;
	std::string query = dpm.select(&ManagedPolicy::value)
						   .join<PolicyDefinition>()
						   .on(expr(&ManagedPolicy::pid) == expr(&PolicyDefinition::id))
						   .join<Admin>()
						   .on(expr(&ManagedPolicy::aid) == expr(&Admin::id))
						   .where(expr(&ManagedPolicy::pid) == id);

	std::string expect = "SELECT managed_policy.value FROM managed_policy " \
						 "INNER JOIN policy_definition ON managed_policy.pid = policy_definition.id " \
						 "INNER JOIN admin ON managed_policy.aid = admin.id " \
						 "WHERE managed_policy.pid = ?";

	TEST_EXPECT(expect, query);
}

TESTCASE(UPDATE)
{
	std::string name;
	int uid = 0;
	std::string selectQuery = admin.select(&Admin::id).where(expr(&Admin::pkg) == name &&
															 expr(&Admin::uid) == uid);

	std::string selectExpect = "SELECT id FROM admin WHERE pkg = ? AND uid = ?";

	TEST_EXPECT(selectExpect, selectQuery);

	int id = 0, aid = 0;
	std::string updateQuery = managedPolicy.update(&ManagedPolicy::value)
										   .where(expr(&ManagedPolicy::pid) == id &&
												  expr(&ManagedPolicy::aid) == aid);

	std::string updateExpect = "UPDATE managed_policy SET value = ? WHERE pid = ? AND aid = ?";

	TEST_EXPECT(updateExpect, updateQuery);
}

TESTCASE(ENROLL)
{
	std::string name;
	int uid = 0;
	std::string selectQuery = admin.selectAll().where(expr(&Admin::pkg) == name &&
													  expr(&Admin::uid) == uid);

	std::string selectExpect = "SELECT * FROM admin WHERE pkg = ? AND uid = ?";

	TEST_EXPECT(selectExpect, selectQuery);

	std::string insertQuery = admin.insert(&Admin::pkg, &Admin::uid,
										   &Admin::key, &Admin::removable);

	std::string insertExpect = "INSERT INTO admin (pkg, uid, key, removable) VALUES (?, ?, ?, ?)";

	TEST_EXPECT(insertExpect, insertQuery);
}

TESTCASE(UNENROLL)
{
	std::string name;
	int uid = 0;
	std::string query = admin.remove().where(expr(&Admin::pkg) == name &&
											 expr(&Admin::uid) == uid);

	std::string expect = "DELETE FROM admin WHERE pkg = ? AND uid = ?";

	TEST_EXPECT(expect, query);
}

TESTCASE(FETCH_DOMAIN)
{
	std::string query = admin.select(distinct(&Admin::uid));

	std::string expect = "SELECT DISTINCT uid FROM admin";

	TEST_EXPECT(expect, query);
}
