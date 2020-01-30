/*
 *  Copyright (c) 2017-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/query-builder.hpp>

#include <gtest/gtest.h>

using namespace vist::tsqb;

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

Table admin { "admin", Column("id", &Admin::id),
					   Column("pkg", &Admin::pkg),
					   Column("uid", &Admin::uid),
					   Column("key", &Admin::key),
					   Column("removable", &Admin::removable) };

Table managedPolicy { "managed_policy", Column("id", &ManagedPolicy::id),
										Column("aid", &ManagedPolicy::aid),
										Column("pid", &ManagedPolicy::pid),
										Column("value", &ManagedPolicy::value) };

Table policyDefinition { "policy_definition", Column("id", &PolicyDefinition::id),
											  Column("scope", &PolicyDefinition::scope),
											  Column("name", &PolicyDefinition::name),
											  Column("ivalue", &PolicyDefinition::ivalue) };

Database db { "dpm", admin, managedPolicy, policyDefinition };

class TsqbTests : public testing::Test {};

TEST_F(TsqbTests, SELECT)
{
	std::string select1 = admin.select(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string select2 = admin.select(&Admin::id, &Admin::uid, &Admin::key);

	EXPECT_EQ(select1, "SELECT id, pkg, uid, key FROM admin");
	EXPECT_EQ(select2, "SELECT id, uid, key FROM admin");
}

TEST_F(TsqbTests, SELECT_ALL)
{
	std::string select = admin.selectAll();

	EXPECT_EQ(select, "SELECT * FROM admin");
}

TEST_F(TsqbTests, SELECT_WHERE)
{
	std::string select1 = admin.select(&Admin::uid, &Admin::key)
							   .where(expr(&Admin::id) > 3);
	std::string select2 = admin.selectAll().where(expr(&Admin::uid) > 3);
	std::string select3 = admin.selectAll().where(expr(&Admin::uid) > 3 &&
												  expr(&Admin::pkg) == "dpm");
	std::string select4 = admin.selectAll().where(expr(&Admin::uid) > 3 ||
												  expr(&Admin::pkg) == "dpm");

	EXPECT_EQ(select1, "SELECT uid, key FROM admin WHERE id > ?");
	EXPECT_EQ(select2, "SELECT * FROM admin WHERE uid > ?");
	EXPECT_EQ(select3, "SELECT * FROM admin WHERE uid > ? AND pkg = ?");
	EXPECT_EQ(select4, "SELECT * FROM admin WHERE uid > ? OR pkg = ?");
}

TEST_F(TsqbTests, SELECT_DISTINCT)
{
	std::string select = admin.select(distinct(&Admin::uid, &Admin::key))
							   .where(expr(&Admin::id) > 3);

	EXPECT_EQ(select, "SELECT DISTINCT uid, key FROM admin WHERE id > ?");
}

TEST_F(TsqbTests, UPDATE)
{
	int uid = 0, id = 1;
	std::string update1 = admin.update(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string update2 = admin.update(&Admin::key).where(expr(&Admin::uid) == uid &&
														  expr(&Admin::id) == id);
	std::string update3 = admin.update(&Admin::key, &Admin::pkg)
							   .where(expr(&Admin::uid) == 0 && expr(&Admin::id) == 1);

	EXPECT_EQ(update1, "UPDATE admin SET id = ?, pkg = ?, uid = ?, key = ?");
	EXPECT_EQ(update2, "UPDATE admin SET key = ? WHERE uid = ? AND id = ?");
	EXPECT_EQ(update3, "UPDATE admin SET key = ?, pkg = ? WHERE uid = ? AND id = ?");
}

TEST_F(TsqbTests, DELETE)
{
	std::string delete1 = admin.remove();
	std::string delete2 = admin.remove().where(expr(&Admin::pkg) == "dpm" &&
											   expr(&Admin::uid) == 3);

	EXPECT_EQ(delete1, "DELETE FROM admin");
	EXPECT_EQ(delete2, "DELETE FROM admin WHERE pkg = ? AND uid = ?");
}

TEST_F(TsqbTests, INSERT)
{
	std::string insert1 = admin.insert(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string insert2 = admin.insert(&Admin::id, &Admin::pkg, &Admin::key);

	EXPECT_EQ(insert1, "INSERT INTO admin (id, pkg, uid, key) VALUES (?, ?, ?, ?)");
	EXPECT_EQ(insert2, "INSERT INTO admin (id, pkg, key) VALUES (?, ?, ?)");
}

TEST_F(TsqbTests, TYPE_SAFE)
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

TEST_F(TsqbTests, MULTI_SELECT)
{
	std::string multiSelect1 = db.select(&Admin::uid, &Admin::key,
										 &ManagedPolicy::id, &ManagedPolicy::value);
	std::string multiSelect2 = db.select(&Admin::uid, &Admin::key,
										 &ManagedPolicy::id, &ManagedPolicy::value)
								 .where(expr(&Admin::uid) > 0 && expr(&ManagedPolicy::id) == 3);

	EXPECT_EQ(multiSelect1, "SELECT admin.uid, admin.key, managed_policy.id, "
							"managed_policy.value FROM admin, managed_policy");
	EXPECT_EQ(multiSelect2, "SELECT admin.uid, admin.key, managed_policy.id, "
							"managed_policy.value FROM admin, managed_policy "
							"WHERE admin.uid > ? AND managed_policy.id = ?");
}

TEST_F(TsqbTests, JOIN)
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

	EXPECT_EQ(join1, "SELECT admin.uid, admin.key FROM admin "
					 "LEFT OUTER JOIN policy_definition");
	EXPECT_EQ(join2, "SELECT admin.uid, admin.key FROM admin "
					 "CROSS JOIN managed_policy");
	EXPECT_EQ(join3, "SELECT managed_policy.value FROM managed_policy "
					 "INNER JOIN policy_definition "
					 "ON managed_policy.pid = policy_definition.id "
					 "INNER JOIN admin ON managed_policy.aid = admin.id "
					 "WHERE managed_policy.pid = ?");
}
