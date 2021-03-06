/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <klay/testbench.h>

#include <iostream>

using namespace query_builder;

struct Admin {
	int id;
	std::string pkg;
	int uid;
	std::string key;
	int removable;
};

struct ManagedPolicy {
	int id;
	int aid;
	int pid;
	int value;
};

struct PolicyDefinition {
	int id;
	int scope;
	std::string name;
	int ivalue;
};

auto admin = make_table("admin", make_column("id", &Admin::id),
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

auto db = make_database("dpm", admin, managedPolicy, policyDefinition);

TESTCASE(SELECT)
{
	std::string select1 = admin.select(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string select2 = admin.select(&Admin::id, &Admin::uid, &Admin::key);

	TEST_EXPECT(select1, "SELECT id, pkg, uid, key FROM admin");
	TEST_EXPECT(select2, "SELECT id, uid, key FROM admin");
}

TESTCASE(SELECT_ALL)
{
	std::string select = admin.selectAll();

	TEST_EXPECT(select, "SELECT * FROM admin");
}

TESTCASE(SELECT_WHERE)
{
	std::string select1 = admin.select(&Admin::uid, &Admin::key)
							   .where(expr(&Admin::id) > 3);
	std::string select2 = admin.selectAll().where(expr(&Admin::uid) > 3);
	std::string select3 = admin.selectAll().where(expr(&Admin::uid) > 3 &&
												  expr(&Admin::pkg) == "dpm");
	std::string select4 = admin.selectAll().where(expr(&Admin::uid) > 3 ||
												  expr(&Admin::pkg) == "dpm");

	TEST_EXPECT(select1, "SELECT uid, key FROM admin WHERE id > ?");
	TEST_EXPECT(select2, "SELECT * FROM admin WHERE uid > ?");
	TEST_EXPECT(select3, "SELECT * FROM admin WHERE uid > ? AND pkg = ?");
	TEST_EXPECT(select4, "SELECT * FROM admin WHERE uid > ? OR pkg = ?");
}

TESTCASE(SELECT_DISTINCT)
{
	std::string select = admin.select(distinct(&Admin::uid, &Admin::key))
							   .where(expr(&Admin::id) > 3);

	TEST_EXPECT(select, "SELECT DISTINCT uid, key FROM admin WHERE id > ?");
}

TESTCASE(UPDATE)
{
	int uid = 0, id = 1;
	std::string update1 = admin.update(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string update2 = admin.update(&Admin::key).where(expr(&Admin::uid) == uid &&
														  expr(&Admin::id) == id);
	std::string update3 = admin.update(&Admin::key, &Admin::pkg)
							   .where(expr(&Admin::uid) == 0 && expr(&Admin::id) == 1);

	TEST_EXPECT(update1, "UPDATE admin SET id = ?, pkg = ?, uid = ?, key = ?");
	TEST_EXPECT(update2, "UPDATE admin SET key = ? WHERE uid = ? AND id = ?");
	TEST_EXPECT(update3, "UPDATE admin SET key = ?, pkg = ? WHERE uid = ? AND id = ?");
}

TESTCASE(DELETE)
{
	std::string delete1 = admin.remove();
	std::string delete2 = admin.remove().where(expr(&Admin::pkg) == "dpm" &&
											   expr(&Admin::uid) == 3);

	TEST_EXPECT(delete1, "DELETE FROM admin");
	TEST_EXPECT(delete2, "DELETE FROM admin WHERE pkg = ? AND uid = ?");
}

TESTCASE(INSERT)
{
	std::string insert1 = admin.insert(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string insert2 = admin.insert(&Admin::id, &Admin::pkg, &Admin::key);

	TEST_EXPECT(insert1, "INSERT INTO admin (id, pkg, uid, key) VALUES (?, ?, ?, ?)");
	TEST_EXPECT(insert2, "INSERT INTO admin (id, pkg, key) VALUES (?, ?, ?)");
}

TESTCASE(TYPE_SAFE)
{
/*
 * Below cause complie error since expression types are dismatch.

	std::string type_unsafe1 = admin.selectAll().where(expr(&Admin::uid) > "dpm");
	std::string type_unsafe2 = admin.selectAll().where(expr(&Admin::uid) == "dpm");
	std::string type_unsafe3 = admin.selectAll().where(expr(&Admin::pkg) == 3);
	int pkg = 3;
	std::string type_unsafe4 = admin.selectAll().where(expr(&Admin::pkg) < pkg);
	std::string type_unsafe5 = admin.remove().where(expr(&Admin::pkg) == "dpm" &&
													expr(&Admin::uid) == "dpm");
*/
}

TESTCASE(MULTI_SELECT)
{
	std::string multiSelect1 = db.select(&Admin::uid, &Admin::key,
										 &ManagedPolicy::id, &ManagedPolicy::value);
	std::string multiSelect2 = db.select(&Admin::uid, &Admin::key,
										 &ManagedPolicy::id, &ManagedPolicy::value)
								 .where(expr(&Admin::uid) > 0 && expr(&ManagedPolicy::id) == 3);

	TEST_EXPECT(multiSelect1, "SELECT admin.uid, admin.key, managed_policy.id, "
							  "managed_policy.value FROM admin, managed_policy");
	TEST_EXPECT(multiSelect2, "SELECT admin.uid, admin.key, managed_policy.id, "
							  "managed_policy.value FROM admin, managed_policy "
							  "WHERE admin.uid > ? AND managed_policy.id = ?");
}

TESTCASE(JOIN)
{
	std::string join1 = db.select(&Admin::uid, &Admin::key)
						  .join<PolicyDefinition>(condition::Join::LEFT_OUTER);
	std::string join2 = db.select(&Admin::uid, &Admin::key)
						  .join<ManagedPolicy>(condition::Join::CROSS);
	std::string join3 = db.select(&ManagedPolicy::value)
						  .join<PolicyDefinition>()
						  .on(expr(&ManagedPolicy::pid) == expr(&PolicyDefinition::id))
						  .join<Admin>()
						  .on(expr(&ManagedPolicy::aid) == expr(&Admin::id))
						  .where(expr(&ManagedPolicy::pid) == 99);

	TEST_EXPECT(join1, "SELECT admin.uid, admin.key FROM admin "
					   "LEFT OUTER JOIN policy_definition");
	TEST_EXPECT(join2, "SELECT admin.uid, admin.key FROM admin "
					   "CROSS JOIN managed_policy");
	TEST_EXPECT(join3, "SELECT managed_policy.value FROM managed_policy "
					   "INNER JOIN policy_definition "
					   "ON managed_policy.pid = policy_definition.id "
					   "INNER JOIN admin ON managed_policy.aid = admin.id "
					   "WHERE managed_policy.pid = ?");
}
