/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <utility>
#include <iostream>
#include <stdexcept>

#include <klay/rmi/client.h>
#include <klay/rmi/service.h>
#include <klay/audit/logger.h>
#include <klay/file-descriptor.h>

#include <klay/testbench.h>

namespace {

const std::string IPC_TEST_ADDRESS = "/tmp/.dpm-test";

void AuditTrail(const rmi::Credentials& cred, const std::string& method, int condition)
{
	std::cout << "AuditTrail pid: " << cred.pid << " method: " << method << std::endl;
}

};

class TestServer {
public:
	TestServer()
	{
		service.reset(new rmi::Service(IPC_TEST_ADDRESS));

		service->expose(this, "", (std::string)(TestServer::method1)(std::string));
		service->expose(this, "", (std::string)(TestServer::method2)(std::string, std::string));
		service->expose(this, "", (std::string)(TestServer::method3)(std::string, std::string, std::string));
		service->expose(this, "", (std::string)(TestServer::method4)(std::string, std::string, std::string, std::string));

		service->expose(this, "", (klay::FileDescriptor)(TestServer::signalProvider)(std::string));
		service->expose(this, "", (klay::FileDescriptor)(TestServer::policyNotificationProvider)(std::string));

		service->expose(this, "", (int)(TestServer::sendSignal)());
		service->expose(this, "", (int)(TestServer::sendPolicyChangeNotification)());

		service->setAuditTrail(AuditTrail);

		service->createNotification("TestPolicyChanged");
		service->createNotification("TestSignal");
	}

	void run()
	{
		service->start();
	}

	std::string method1(std::string& arg1)
	{
		return std::string("Method1 result");
	}

	std::string method2(std::string& arg1, std::string& arg2)
	{
		return std::string("Method2 result");
	}

	std::string method3(std::string& arg1, std::string& arg2, std::string& arg3)
	{
		return std::string("Method3 result");
	}

	std::string method4(std::string& arg1, std::string& arg2, std::string& arg3, std::string& arg4)
	{
		return std::string("Method4 result");
	}

	int sendPolicyChangeNotification()
	{
		service->notify("TestPolicyChanged", 1234);
		return 0;
	}

	int sendSignal()
	{
		service->notify("TestSignal");
		return 0;
	}

	klay::FileDescriptor signalProvider(const std::string& name)
	{
		return service->subscribeNotification(name);
	}

	klay::FileDescriptor policyNotificationProvider(const std::string& name)
	{
		return service->subscribeNotification(name);
	}

private:
	std::unique_ptr<rmi::Service> service;
};

class TestClient {
public:
	TestClient() :
		signalTriggered(false),
		policyChangeNotificationTriggered(false)
	{
	}

	void connect()
	{
		auto policyChangedListener = [this](const std::string& name, int value) {
			policyChangeNotificationTriggered = true;
		};

		auto policySignalListener = [this](const std::string& name) {
			signalTriggered = true;
		};

		client.reset(new rmi::Client(IPC_TEST_ADDRESS));
		client->connect();

		client->subscribe<std::string, int>("TestServer::policyNotificationProvider",
											"TestPolicyChanged", policyChangedListener);
		client->subscribe<std::string>("TestServer::signalProvider",
									   "TestSignal", policySignalListener);
	}

	void disconnect()
	{
		client.reset();
	}

	std::string method1(std::string& arg1)
	{
		return client->methodCall<std::string>("TestServer::method1", arg1);
	}

	std::string method2(std::string& arg1, std::string& arg2)
	{
		return client->methodCall<std::string>("TestServer::method2", arg1, arg2);
	}

	std::string method3(std::string& arg1, std::string& arg2, std::string& arg3)
	{
		return client->methodCall<std::string>("TestServer::method3", arg1, arg2, arg3);
	}

	std::string method4(std::string& arg1, std::string& arg2, std::string& arg3, std::string& arg4)
	{
		return client->methodCall<std::string>("TestServer::method4", arg1, arg2, arg3, arg4);
	}

	void requestSignal()
	{
		signalTriggered = false;
		client->methodCall<int>("TestServer::sendSignal");
		while (!signalTriggered) {
			::sleep(1);
		}
	}

	void requestPolicyChangeNotification()
	{
		policyChangeNotificationTriggered = false;
		client->methodCall<int>("TestServer::sendPolicyChangeNotification");
		while (!policyChangeNotificationTriggered) {
			sleep(1);
		}
	}

	std::string invalidMethod(std::string& arg)
	{
		return client->methodCall<std::string>("TestServer::invalidMethod", arg);
	}

private:
	volatile bool signalTriggered;
	volatile bool policyChangeNotificationTriggered;
	std::unique_ptr<rmi::Client> client;
};

int WaitForFile(const std::string& path, const unsigned int timeout)
{
	struct stat st;
	unsigned int loop = 0;

	while (stat(path.c_str(), &st) == -1) {
		if (errno != ENOENT) {
			ERROR(KSINK, "Error on waitting for: " << path);
			return -1;
		}

		if (((++loop) * 100) > timeout) {
			ERROR(KSINK, "Error on waitting for: " << path);
			return -1;
		}

		usleep(100 * 1000);
	}

	return 0;
}

int WaitForPid(pid_t pid)
{
	int status, ret;
	char errmsg[256];

	do {
		ret = waitpid(pid, &status, 0);
		if (ret == -1) {
			if (errno != EINTR) {
				ERROR(KSINK, "Wait Pid failed: " << std::to_string(pid) << "(" <<
							 strerror_r(errno, errmsg, sizeof(errmsg)) << ")");
				return -1;
			}
		}
	} while (ret == EINTR || ret != pid);

	return status;
}

pid_t PrepareTestServer(void)
{
	::unlink(IPC_TEST_ADDRESS.c_str());

	pid_t pid = fork();

	if (pid < 0) {
		return -1;
	}

	if (pid == 0) {
		try {
			TestServer server;
			server.run();
		} catch (std::exception& e) {
			ERROR(KSINK, e.what());
			return -1;
		}

		return 0;
	}

	return pid;
}

class IpcTestSuite : public testbench::TestSuite {
public:
	IpcTestSuite(const std::string& name) :
		testbench::TestSuite(name), pid(-1)
	{
		addTest(IpcTestSuite::connectionTest);
		addTest(IpcTestSuite::remoteMethodCallTest);
		addTest(IpcTestSuite::notificationTest);
	}

	void setup()
	{
		pid = PrepareTestServer();
		if (pid == -1) {
			ERROR(KSINK, "Preparing test server failed");
			return;
		}

		WaitForFile(IPC_TEST_ADDRESS, 1000);
	}

	void teardown()
	{
		if (::kill(pid, SIGTERM) == -1) {
			return;
		}

		int status = WaitForPid(pid);
		if (status == -1 || !WIFEXITED(status)) {
			return;
		}
	}

	void connectionTest()
	{
		try {
			TestClient client;
			client.connect();
			client.disconnect();
		} catch (klay::Exception& e) {
			ERROR(KSINK, e.what());
		}
	}

	void notificationTest()
	{
		try {
			TestClient client;
			client.connect();

			client.requestSignal();
			client.requestPolicyChangeNotification();

			client.disconnect();
		} catch (klay::Exception& e) {
			ERROR(KSINK, e.what());
		}
	}

	void remoteMethodCallTest()
	{
		try {
			TestClient client;
			client.connect();

			std::string param1("First Parameter");
			std::string param2("Second Parameter");
			std::string param3("Third Parameter");
			std::string param4("Fourth Parameter");

			std::string result1 = client.method1(param1);
			std::string result2 = client.method2(param1, param2);
			std::string result3 = client.method3(param1, param2, param3);
			std::string result4 = client.method4(param1, param2, param3, param4);

			client.requestSignal();
			client.requestPolicyChangeNotification();

			client.disconnect();
		} catch (klay::Exception& e) {
			ERROR(KSINK, e.what());
		}
	}

private:
	pid_t pid;
};

IpcTestSuite ipcTestSuite("IpcTestSuite");
