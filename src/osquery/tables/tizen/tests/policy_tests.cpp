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

#include <policyd/api.h>

class PolicyTests : public testing::Test {};

TEST_F(PolicyTests, get_all) {
	auto policies = policyd::API::GetAll();

	EXPECT_TRUE(policies.size() > 0);
}

TEST_F(PolicyTests, get_admin_all) {
	auto admins = policyd::API::Admin::GetAll();
	EXPECT_EQ(admins.size(), 0);

	policyd::API::Admin::Enroll("testAdmin", 0);
	admins = policyd::API::Admin::GetAll();
	EXPECT_EQ(admins.size(), 1);

	policyd::API::Admin::Enroll("testAdmin", 1);
	admins = policyd::API::Admin::GetAll();
	EXPECT_EQ(admins.size(), 2);

	policyd::API::Admin::Disenroll("testAdmin", 0);
	admins = policyd::API::Admin::GetAll();
	EXPECT_EQ(admins.size(), 1);

	policyd::API::Admin::Disenroll("testAdmin", 1);
	admins = policyd::API::Admin::GetAll();
	EXPECT_EQ(admins.size(), 0);
}
