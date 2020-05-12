/*
 *  Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <gtest/gtest.h>

#include <vist/json/json.hpp>
#include <vist/logger.hpp>

using namespace vist::json;

TEST(JsonTests, int)
{
	Json json;
	json["int"] = 1;

	int value = json["int"];
	EXPECT_EQ(value, 1);

	json["int"] = -1;
	value = json["int"];
	EXPECT_EQ(value, -1);

	EXPECT_EQ(static_cast<int>(json["int"]), -1);

	EXPECT_EQ(json["int"].serialize(), "-1");
}

TEST(JsonTests, int_type_mismatch)
{
	Json json;
	json["int"] = 1;

	try {
		static_cast<std::string>(json["int"]);
		EXPECT_TRUE(false);
	} catch(...) {
		EXPECT_TRUE(true);
	}
}

TEST(JsonTests, string)
{
	Json json;
	json["string"] = "initial value";

	std::string value = json["string"];
	EXPECT_EQ(value, "initial value");

	json["string"] = "changed value";
	value = static_cast<std::string>(json["string"]);
	EXPECT_EQ(value, "changed value");

	EXPECT_EQ(json["string"].serialize(), "\"changed value\"");
}

TEST(JsonTests, string_type_mismatch)
{
	Json json;
	json["string"] = "initial value";

	try {
		static_cast<int>(json["string"]);
		EXPECT_TRUE(false);
	} catch(...) {
		EXPECT_TRUE(true);
	}
}

TEST(JsonTests, array)
{
	Array array;
	array.push(100);
	array.push("string");

	EXPECT_EQ(array.size(), 2);
	EXPECT_EQ(array.serialize(), "[ 100, \"string\" ]");

	EXPECT_EQ(static_cast<int>(array.at(0)), 100);
	EXPECT_EQ(static_cast<std::string>(array.at(1)), "string");

	Json json;
	json.push("array", array);
	EXPECT_EQ(json.size(), 1);

	auto& result = json.get<Array>("array");
	EXPECT_EQ(json.size(), 1);
	EXPECT_EQ(result.size(), 2);

	EXPECT_EQ(static_cast<int>(result.at(0)), 100);
	EXPECT_EQ(static_cast<std::string>(result.at(1)), "string");
}

TEST(JsonTests, object)
{
	Object object;
	object["int"] = 1;
	object["string"] = "initial value";

	EXPECT_EQ(object.size(), 2);
	EXPECT_EQ(object.serialize(), "{ \"string\": \"initial value\", \"int\": 1 }");

	EXPECT_EQ(static_cast<int>(object["int"]), 1);
	EXPECT_EQ(static_cast<std::string>(object["string"]), "initial value");

	Json json;
	json.push("object", object);
	EXPECT_EQ(json.size(), 1);

	auto result = json.get<Object>("object");
	EXPECT_EQ(static_cast<int>(result["int"]), 1);
	EXPECT_EQ(static_cast<std::string>(result["string"]), "initial value");
}

TEST(JsonTests, serialize)
{
	Json json;
	json["int"] = 1;
	json["string"] = "root value";
	// expected: { "string": "root value", "int": 1 }
	EXPECT_EQ(json.serialize(), "{ \"string\": \"root value\", \"int\": 1 }");

	Object child;
	child["int"] = 2;
	child["string"] = "child value";
	json.push("child", child);

	// Json don't keep order for performance.
	// expected: { "child": { "string": "child value", "int": 2 }, "int": 1, "string": "root value" }
	EXPECT_EQ(json.serialize(),
			  "{ \"child\": { \"string\": \"child value\", "
			  "\"int\": 2 }, \"int\": 1, \"string\": \"root value\" }");
}
