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

#include <vist/ipc/client.hpp>
#include <vist/ipc/server.hpp>

#include <chrono>
#include <thread>

#include <sys/types.h>
#include <unistd.h>

using namespace vist;

namespace {
	std::string g_socket = "/tmp/vist-test";
} // anonymous namespace

class TestServer {
public:
	void init()
	{
		auto& server = ipc::Server::Instance(g_socket);
		server->expose(this, "", (std::string)(TestServer::testMethod)());
		server->start();
	}

	std::string testMethod()
	{
		return "test";
	}
};

class IpcTests : public testing::Test {
public:
	void SetUp() override
	{
		::unlink(g_socket.c_str());

		this->pid = fork();
		if (pid < 0) {
			return;
		} else if (pid == 0) {
			TestServer server;
			server.init();
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	void TearDown() override
	{
		if (::kill(pid, SIGTERM) == -1)
			return;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

private:
	pid_t pid = -1;
};

TEST_F(IpcTests, method_call) {
	auto& client = ipc::Client::Instance(g_socket);

	std::string ret = client->methodCall<std::string>("TestServer::testMethod");
	EXPECT_EQ(ret, "test");
}
