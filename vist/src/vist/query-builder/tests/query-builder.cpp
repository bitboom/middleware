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

	DECLARE_COLUMN(Id, "id", &Admin::id);
	DECLARE_COLUMN(Pkg, "pkg", &Admin::pkg);
	DECLARE_COLUMN(Uid, "uid", &Admin::uid);
	DECLARE_COLUMN(Key, "key", &Admin::key);
	DECLARE_COLUMN(Removable, "removable", &Admin::removable);
};

struct ManagedPolicy {
	int id;
	int aid;
	int pid;
	int value;

	DECLARE_COLUMN(Id, "id", &ManagedPolicy::id);
	DECLARE_COLUMN(Aid, "aid", &ManagedPolicy::aid);
	DECLARE_COLUMN(Pid, "pid", &ManagedPolicy::pid);
	DECLARE_COLUMN(Value, "value", &ManagedPolicy::value);
};

struct PolicyDefinition {
	int id;
	int scope;
	std::string name;
	int ivalue;

	DECLARE_COLUMN(Id, "id", &PolicyDefinition::id);
	DECLARE_COLUMN(Scope, "scope", &PolicyDefinition::scope);
	DECLARE_COLUMN(Name, "name", &PolicyDefinition::name);
	DECLARE_COLUMN(Ivalue, "ivalue", &PolicyDefinition::ivalue);
};

DECLARE_TABLE(AdminTable, "admin", Admin::Id, Admin::Pkg,
			  Admin::Uid, Admin::Key, Admin::Removable);

DECLARE_TABLE(ManagedPolicyTable, "managed_policy", ManagedPolicy::Id,
			  ManagedPolicy::Aid,
			  ManagedPolicy::Pid,
			  ManagedPolicy::Value);

DECLARE_TABLE(PolicyDefinitionTable, "policy_definition", PolicyDefinition::Id,
			  PolicyDefinition::Scope,
			  PolicyDefinition::Name,
			  PolicyDefinition::Ivalue);

DECLARE_DATABASE(DPM, "dpm", AdminTable, ManagedPolicyTable, PolicyDefinitionTable);

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
	std::string select2 = AdminTable.selectAll().where(Admin::Uid > 3);
	std::string select3 = AdminTable.selectAll().where(Admin::Uid > 3 && Admin::Pkg == "dpm");
	std::string select4 = AdminTable.selectAll().where(Admin::Uid > 3 || Admin::Pkg == "dpm");

	EXPECT_EQ(select1, "SELECT uid, key FROM admin WHERE id > 3");
	EXPECT_EQ(select2, "SELECT * FROM admin WHERE uid > 3");
	EXPECT_EQ(select3, "SELECT * FROM admin WHERE uid > 3 AND pkg = 'dpm'");
	EXPECT_EQ(select4, "SELECT * FROM admin WHERE uid > 3 OR pkg = 'dpm'");
}

TEST(QueryBuilderTsqbTests, UPDATE)
{
	int uid = 0, id = 1;
	std::string update1 = AdminTable.update(Admin::Id = id, Admin::Pkg = "pkg",
											Admin::Uid = uid, Admin::Key = "key");
	std::string update2 = AdminTable.update(Admin::Key = "key")
						  .where((Admin::Uid == uid) && (Admin::Id == id));
	std::string update3 = AdminTable.update(Admin::Key = "key", Admin::Pkg = "pkg")
						  .where((Admin::Uid == 0) && (Admin::Id == 1));

	EXPECT_EQ(update1, "UPDATE admin SET id = 1, pkg = 'pkg', uid = 0, key = 'key'");
	EXPECT_EQ(update2, "UPDATE admin SET key = 'key' WHERE uid = 0 AND id = 1");
	EXPECT_EQ(update3, "UPDATE admin SET key = 'key', pkg = 'pkg' WHERE uid = 0 AND id = 1");
}

TEST(QueryBuilderTsqbTests, DELETE)
{
	std::string delete1 = AdminTable.remove();
	std::string delete2 = AdminTable.remove().where((Admin::Pkg == "dpm") && (Admin::Uid == 3));

	EXPECT_EQ(delete1, "DELETE FROM admin");
	EXPECT_EQ(delete2, "DELETE FROM admin WHERE pkg = 'dpm' AND uid = 3");
}

TEST(QueryBuilderTsqbTests, INSERT)
{
	int id = 0;
	std::string pkg = "pkg";
	int uid = 1;
	std::string key = "key";

	std::string insert1 = AdminTable.insert(Admin::Id = id,
											Admin::Pkg = pkg,
											Admin::Uid = uid,
											Admin::Key = key);
	std::string insert2 = AdminTable.insert(Admin::Id = id,
											Admin::Pkg = pkg,
											Admin::Key = key);

	EXPECT_EQ(insert1, "INSERT INTO admin (id, pkg, uid, key) "
			  "VALUES (0, 'pkg', 1, 'key')");
	EXPECT_EQ(insert2, "INSERT INTO admin (id, pkg, key) "
			  "VALUES (0, 'pkg', 'key')");
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
	std::string multiSelect1 = DPM.select(Admin::Uid, Admin::Key,
										  ManagedPolicy::Id, ManagedPolicy::Value);
	std::string multiSelect2 = DPM.select(Admin::Uid, Admin::Key,
										  ManagedPolicy::Id, ManagedPolicy::Value)
							   .where((Admin::Uid > 0) && (ManagedPolicy::Id == 3));

	EXPECT_EQ(multiSelect1, "SELECT admin.uid, admin.key, managed_policy.id, "
			  "managed_policy.value FROM admin, managed_policy");
	EXPECT_EQ(multiSelect2, "SELECT admin.uid, admin.key, managed_policy.id, "
			  "managed_policy.value FROM admin, managed_policy "
			  "WHERE admin.uid > 0 AND managed_policy.id = 3");
}
