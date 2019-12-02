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

#include <vist/logger.hpp>
#include <vist/logger/glog.hpp>

using namespace vist;

TEST(LoggerTests, default_)
{
	INFO(VIST) << "Info message" << 1;
	DEBUG(VIST) << "Debug message" << 2 << 'a';
	WARN(VIST) << "Warn message" << 3 << 'b' << true;
	ERROR(VIST) << "Error message" << 4 << 'c' << false << 0.0f;
}

TEST(LoggerTests, console)
{
	LogStream::Init(std::make_shared<Console>());
	INFO(VIST) << "Info message" << 1;
	DEBUG(VIST) << "Debug message" << 2 << 'a';
	WARN(VIST) << "Warn message" << 3 << 'b' << true;
	ERROR(VIST) << "Error message" << 4 << 'c' << false << 0.0f;
}

TEST(LoggerTests, glog)
{
	LogStream::Init(std::make_shared<Glog>());
	INFO(VIST) << "Info message" << 1;
	DEBUG(VIST) << "Debug message" << 2 << 'a';
	WARN(VIST) << "Warn message" << 3 << 'b' << true;
	ERROR(VIST) << "Error message" << 4 << 'c' << false << 0.0f;
}
