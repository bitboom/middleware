/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/result.hpp>

#include <string>
#include <vector>

class ResultTests : public testing::Test {};

using namespace vist;

enum class FileErr {
	InvalidArgument,
	NotFound
};

Result<std::string, FileErr> readSuccess()
{
	return std::string("File contents");
}

Result<std::string, FileErr> readFail()
{
	return FileErr::NotFound;
}

Result<std::vector<int>, FileErr> readVector()
{
	std::vector<int> v{1, 2, 3};
	return v;
}

TEST_F(ResultTests, success)
{
	EXPECT_TRUE(readSuccess().ok());
	EXPECT_TRUE(!readSuccess().err());
	EXPECT_NE(std::string::npos, readSuccess().get().find("File"));
}

TEST_F(ResultTests, fail)
{
	EXPECT_TRUE(readFail().err());
	EXPECT_TRUE(!readFail().ok());
	EXPECT_EQ(FileErr::NotFound, readFail().getErrCode());
}

TEST_F(ResultTests, vector)
{
	EXPECT_TRUE(readVector().ok());
	EXPECT_TRUE(!readVector().err());
	EXPECT_EQ(3, readVector().get().size());
}
