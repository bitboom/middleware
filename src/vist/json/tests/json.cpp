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

	json["int"].deserialize("1");
	EXPECT_EQ(static_cast<int>(json["int"]), 1);
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

	json["string"].deserialize("\"deserialized value\"");
	EXPECT_EQ(static_cast<std::string>(json["string"]), "deserialized value");
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

	EXPECT_EQ(static_cast<int>(array.at(0)), 100);
	EXPECT_EQ(static_cast<std::string>(array.at(1)), "string");

	auto serialized = array.serialize();
	EXPECT_EQ(serialized, "[ 100, \"string\" ]");

	Array restore;
	restore.deserialize(serialized);
	EXPECT_EQ(static_cast<int>(restore.at(0)), 100);
	EXPECT_EQ(static_cast<std::string>(restore.at(1)), "string");

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

	EXPECT_EQ(static_cast<int>(object["int"]), 1);
	EXPECT_EQ(static_cast<std::string>(object["string"]), "initial value");

	std::string serialized = object.serialize();
	EXPECT_EQ(serialized, "{ \"string\": \"initial value\", \"int\": 1 }");

	Object restore;
	restore.deserialize(serialized);
	EXPECT_EQ(static_cast<int>(restore["int"]), 1);
	EXPECT_EQ(static_cast<std::string>(restore["string"]), "initial value");

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

	Object object;
	object["int"] = 2;
	object["string"] = "child value";
	json.push("object", object);

	Array array;
	array.push(3);
	array.push("array value");
	json.push("array", array);

	// Json don't keep order for performance.
	// expected: { "array": [ 3, "array value" ],
	//             "object": { "string": "child value", "int": 2 },
	//             "int": 1,
	//             "string": "root value" }
	auto serialized = json.serialize();
	EXPECT_EQ(serialized, "{ \"array\": [ 3, \"array value\" ], "
						  "\"object\": { \"string\": \"child value\", \"int\": 2 }, "
						  "\"int\": 1, "
						  "\"string\": \"root value\" }");

	Json restore;
	restore.deserialize(serialized);

	EXPECT_EQ(static_cast<int>(restore["int"]), 1);
	EXPECT_EQ(static_cast<std::string>(restore["string"]), "root value");

	Object child1 = restore.get<Object>("object");
	EXPECT_EQ(static_cast<int>(child1["int"]), 2);
	EXPECT_EQ(static_cast<std::string>(child1["string"]), "child value");

	Array child2 = restore.get<Array>("array");
	EXPECT_EQ(static_cast<int>(child2.at(0)), 3);
	EXPECT_EQ(static_cast<std::string>(child2.at(1)), "array value");
}
