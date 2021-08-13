/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/dynamic-loader.hpp>

#include "res/sample-plugin.hpp"

#include <string>

using namespace vist;

TEST(DynamicLoaderTests, load)
{
	std::string path = std::string(PLUGIN_INSTALL_DIR) + "/libtest-plugin.so";
	DynamicLoader loader(path);

	auto factory = loader.load<SamplePlugin::FactoryType>("SampleFactory");
	EXPECT_NE(factory, nullptr);

	auto samplePlugin = (*factory)();
	EXPECT_NE(samplePlugin, nullptr);

	EXPECT_EQ(samplePlugin->test(), "sample-plugin-test");
}

TEST(DynamicLoaderTests, not_exist_path)
{
	try {
		DynamicLoader loader("not_exists");
		EXPECT_TRUE(false);
	} catch (const std::exception&) {
		EXPECT_TRUE(true);
	}
}

TEST(DynamicLoaderTests, not_exist_symbol)
{
	try {
		std::string path = std::string(PLUGIN_INSTALL_DIR) + "/libtest-plugin.so";
		DynamicLoader loader(path);

		loader.load<SamplePlugin::FactoryType>("not_exists");
		EXPECT_TRUE(false);
	} catch (const std::exception&) {
		EXPECT_TRUE(true);
	}
}
