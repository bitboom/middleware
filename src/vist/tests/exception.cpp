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

#include <vist/exception.hpp>

enum class ErrCode {
	InvalidArgument = 1,
	DominError,
	LogicError,
	RuntimeError,
	None
};

class ExceptionTests : public testing::Test {};

TEST_F(ExceptionTests, exception)
{
	bool raised = false;
	ErrCode ec = ErrCode::None;
	std::string msg;

	try {
		THROW(ErrCode::InvalidArgument);
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		ec = e.get();
		msg = e.what();
	}

	EXPECT_TRUE(raised);
	EXPECT_EQ(ec, ErrCode::InvalidArgument);
	EXPECT_NE(std::string::npos, msg.find("ErrCode"));
}

TEST_F(ExceptionTests, exception_msg)
{
	bool raised = false;
	ErrCode ec = ErrCode::None;
	std::string msg;

	try {
		THROW(ErrCode::RuntimeError) << "Additional error message";
	} catch (const vist::Exception<ErrCode>& e) {
		raised = true;
		ec = e.get();
		msg = e.what();
	}

	EXPECT_TRUE(raised);
	EXPECT_EQ(ec, ErrCode::RuntimeError);
	EXPECT_NE(std::string::npos, msg.find("Additional"));
}
