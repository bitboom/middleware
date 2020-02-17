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

	inline static Column Id = { "id", &Admin::id };
	inline static Column Pkg = { "pkg", &Admin::pkg };
	inline static Column Uid = { "uid", &Admin::uid };
	inline static Column Key = { "key", &Admin::key };
	inline static Column Removable = { "removable", &Admin::removable };
};

static Table AdminTable { "admin", Admin::Id, Admin::Pkg,
								   Admin::Uid, Admin::Key, Admin::Removable };

struct ManagedPolicy {
	int id;
	int aid;
	int pid;
	int value;

	inline static Column Id = { "id", &ManagedPolicy::id };
	inline static Column Aid = { "aid", &ManagedPolicy::aid };
	inline static Column Pid = { "pid", &ManagedPolicy::pid };
	inline static Column Value = { "value", &ManagedPolicy::value };
};

static Table ManagedPolicyTable { "managed_policy", ManagedPolicy::Id,
													  ManagedPolicy::Aid,
													  ManagedPolicy::Pid,
													  ManagedPolicy::Value };

struct PolicyDefinition {
	int id;
	int scope;
	std::string name;
	int ivalue;

	inline static Column Id = { "id", &PolicyDefinition::id };
	inline static Column Scope = { "scope", &PolicyDefinition::scope };
	inline static Column Name = { "name", &PolicyDefinition::name };
	inline static Column Ivalue = { "ivalue", &PolicyDefinition::ivalue };
};

static Table PolicyDefinition { "policy_definition", PolicyDefinition::Id,
													 PolicyDefinition::Scope,
													 PolicyDefinition::Name,
													 PolicyDefinition::Ivalue };

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

TEST(QueryBuilderTsqbTests, SELECT)
{
	std::string select1 = AdminTable.select(Admin::Id, Admin::Pkg, Admin::Uid, Admin::Key);
	std::string select2 = AdminTable.select(Admin::Id, Admin::Uid, Admin::Key);

	EXPECT_EQ(select1, "SELECT id, pkg, uid, key FROM admin");
	EXPECT_EQ(select2, "SELECT id, uid, key FROM admin");
}

TEST(QueryBuilderTsqbTests, SELECT_ALL)
{
	std::string select = AdminTable.selectAll();

	EXPECT_EQ(select, "SELECT * FROM admin");
}

TEST(QueryBuilderTsqbTests, SELECT_WHERE)
{
	std::string select1 = AdminTable.select(Admin::Uid, Admin::Key)
									.where(Admin::Id > 3);
	std::string select2 = admin.selectAll().where(Admin::Uid > 3);
	std::string select3 = admin.selectAll().where(Admin::Uid > 3 &&
												  Admin::Pkg == "dpm");
	std::string select4 = admin.selectAll().where(Admin::Uid > 3 ||
												  Admin::Pkg == "dpm");

	EXPECT_EQ(select1, "SELECT uid, key FROM admin WHERE id > 3");
	EXPECT_EQ(select2, "SELECT * FROM admin WHERE uid > 3");
	EXPECT_EQ(select3, "SELECT * FROM admin WHERE uid > 3 AND pkg = 'dpm'");
	EXPECT_EQ(select4, "SELECT * FROM admin WHERE uid > 3 OR pkg = 'dpm'");
}

TEST(QueryBuilderTsqbTests, UPDATE)
{
	int uid = 0, id = 1;
	std::string update1 = admin.update(Admin::Id, Admin::Pkg, Admin::Uid, Admin::Key);
	std::string update2 = admin.update(Admin::Key).where((Admin::Uid == uid) &&
														 (Admin::Id == id));
	std::string update3 = admin.update(Admin::Key, Admin::Pkg)
							   .where((Admin::Uid == 0) && (Admin::Id == 1));

	EXPECT_EQ(update1, "UPDATE admin SET id = ?, pkg = ?, uid = ?, key = ?");
	EXPECT_EQ(update2, "UPDATE admin SET key = ? WHERE uid = 0 AND id = 1");
	EXPECT_EQ(update3, "UPDATE admin SET key = ?, pkg = ? WHERE uid = 0 AND id = 1");
}

TEST(QueryBuilderTsqbTests, DELETE)
{
	std::string delete1 = admin.remove();
	std::string delete2 = admin.remove().where((Admin::Pkg == "dpm") &&
											   (Admin::Uid == 3));

	EXPECT_EQ(delete1, "DELETE FROM admin");
	EXPECT_EQ(delete2, "DELETE FROM admin WHERE pkg = 'dpm' AND uid = 3");
}

TEST(QueryBuilderTsqbTests, INSERT)
{
	std::string insert1 = AdminTable.insert(Admin::Id, Admin::Pkg, Admin::Uid, Admin::Key);
	std::string insert2 = AdminTable.insert(Admin::Id, Admin::Pkg, Admin::Key);

	EXPECT_EQ(insert1, "INSERT INTO admin (id, pkg, uid, key) VALUES (?, ?, ?, ?)");
	EXPECT_EQ(insert2, "INSERT INTO admin (id, pkg, key) VALUES (?, ?, ?)");
}

TEST(QueryBuilderTsqbTests, TYPE_SAFE)
{
/*
 * Below cause complie error since expression types are dismatch.

	std::string type_unsafe1 = admin.selectAll().where(Admin::Uid > "dpm");
	std::string type_unsafe2 = admin.selectAll().where(Admin::Uid == "dpm");
	std::string type_unsafe3 = admin.selectAll().where(Admin::Pkg == 3);
	int pkg = 3;
	std::string type_unsafe4 = admin.selectAll().where(Admin::Pkg) < pkg);
	std::string type_unsafe5 = admin.remove().where(Admin::Pkg) == "dpm" &&
													Admin::Uid) == "dpm");
*/
}

TEST(QueryBuilderTsqbTests, MULTI_SELECT)
{
	std::string multiSelect1 = db.select(Admin::Uid, Admin::Key,
										 ManagedPolicy::Id, ManagedPolicy::Value);
	std::string multiSelect2 = db.select(Admin::Uid, Admin::Key,
										 ManagedPolicy::Id, ManagedPolicy::Value)
								 .where((Admin::Uid > 0) && (ManagedPolicy::Id == 3));

	EXPECT_EQ(multiSelect1, "SELECT admin.uid, admin.key, managed_policy.id, "
							"managed_policy.value FROM admin, managed_policy");
	EXPECT_EQ(multiSelect2, "SELECT admin.uid, admin.key, managed_policy.id, "
							"managed_policy.value FROM admin, managed_policy "
							"WHERE admin.uid > 0 AND managed_policy.id = 3");
}

/*
TEST(QueryBuilderTsqbTests, JOIN)
{
	std::string join1 = db.select(Admin::Uid, Admin::Key)
						  .join<PolicyDefinition>(condition::Join::LEFT_OUTER);
	std::string join2 = db.select(Admin::Uid, Admin::Key)
						  .join<ManagedPolicy>(condition::Join::CROSS);
	std::string join3 = db.select(ManagedPolicy::Value)
						  .join<PolicyDefinition>()
						  .on(ManagedPolicy::Pid) == PolicyDefinition::Id))
						  .join<Admin>()
						  .on(ManagedPolicy::Aid) == Admin::Id))
						  .where(ManagedPolicy::Pid) == 99);

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
*/
