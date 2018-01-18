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

auto admin = make_table("admin", make_column("id", &Admin::id),
								 make_column("pkg", &Admin::pkg),
								 make_column("uid", &Admin::uid),
								 make_column("key", &Admin::key),
								 make_column("removable", &Admin::removable));

TESTCASE(SELECT)
{
	std::string select1 = admin.select(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string select2 = admin.select(&Admin::id, &Admin::uid, &Admin::key);

	TEST_EXPECT(true, select1 == "SELECT id pkg uid key FROM admin");
	TEST_EXPECT(true, select2 == "SELECT id uid key FROM admin");
}

TESTCASE(SELECT_ALL)
{
	std::string select = admin.selectAll();

	TEST_EXPECT(true, select == "SELECT * FROM admin");
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

	TEST_EXPECT(true, select1 == "SELECT uid key FROM admin WHERE id > ?");
	TEST_EXPECT(true, select2 == "SELECT * FROM admin WHERE uid > ?");
	TEST_EXPECT(true, select3 == "SELECT * FROM admin WHERE uid > ? AND pkg = ?");
	TEST_EXPECT(true, select4 == "SELECT * FROM admin WHERE uid > ? OR pkg = ?");
}

TESTCASE(SELECT_DISTINCT)
{
	std::string select = admin.select(distinct(&Admin::uid, &Admin::key))
							   .where(expr(&Admin::id) > 3);

	TEST_EXPECT(true, select == "SELECT DISTINCT uid key FROM admin WHERE id > ?");
}

TESTCASE(UPDATE)
{
	int uid = 0, id = 1;
	std::string update1 = admin.update(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string update2 = admin.update(&Admin::key).where(expr(&Admin::uid) == uid &&
														  expr(&Admin::id) == id);
	std::string update3 = admin.update(&Admin::key, &Admin::pkg)
							   .where(expr(&Admin::uid) == 0 && expr(&Admin::id) == 1);

	TEST_EXPECT(true, update1 == "UPDATE admin SET id = ? pkg = ? uid = ? key = ?");
	TEST_EXPECT(true, update2 == "UPDATE admin SET key = ? WHERE uid = ? AND id = ?");
	TEST_EXPECT(true, update3 == "UPDATE admin SET key = ? pkg = ? WHERE uid = ? AND id = ?");
}

TESTCASE(DELETE)
{
	std::string delete1 = admin.remove();
	std::string delete2 = admin.remove().where(expr(&Admin::pkg) == "dpm" &&
											   expr(&Admin::uid) == 3);

	TEST_EXPECT(true, delete1 == "DELETE FROM admin");
	TEST_EXPECT(true, delete2 == "DELETE FROM admin WHERE pkg = ? AND uid = ?");
}

TESTCASE(INSERT)
{
	std::string insert1 = admin.insert(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string insert2 = admin.insert(&Admin::id, &Admin::pkg, &Admin::key);

	TEST_EXPECT(true, insert1 == "INSERT INTO admin (id, pkg, uid, key) VALUES (?, ?, ?, ?)");
	TEST_EXPECT(true, insert2 == "INSERT INTO admin (id, pkg, key) VALUES (?, ?, ?)");
}
