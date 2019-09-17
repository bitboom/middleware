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

#include "pil/policy-event.h"
#include "pil/policy-client.h"

#include <klay/testbench.h>

#include <klay/gmainloop.h>

#include <string>
#include <memory>
#include <thread>
#include <chrono>

namespace {

const std::string TEST_PIL_EVENT_NAME = "EVENT_NAME";
const std::string TEST_PIL_EVENT_STATE = "EVENT_STATE";
std::string TEST_PIL_EVENT_DATA = "EVENT_DATA";

} // anonymous namespace

TESTCASE(PIL_EVENT)
{
	ScopedGMainLoop mainloop;

	//TODO: handle should be created by dpm_manager_create()
	std::unique_ptr<DevicePolicyClient> handle(
		[]() -> DevicePolicyClient* {
			DevicePolicyClient* client = new DevicePolicyClient();

			if (client == nullptr)
				return nullptr;

			return client;
		}());
	TEST_EXPECT(true, handle != nullptr);

	DevicePolicyClient &client = GetDevicePolicyClient(static_cast<void*>(handle.get()));

	bool isCalled = false;
	auto onEvent = [&isCalled](const char* name, const char* state, void* data) {
		isCalled = true;
		auto receivedData = static_cast<std::string*>(data);
		std::cout << "Signal caught. name = " << name << ", state : " << state
				  << ", data : " << *receivedData << std::endl;
	};

	client.subscribeSignal(TEST_PIL_EVENT_NAME, onEvent,
						   static_cast<void*>(&TEST_PIL_EVENT_DATA));

	PolicyEventNotifier::create(TEST_PIL_EVENT_NAME);
	PolicyEventNotifier::emit(TEST_PIL_EVENT_NAME, TEST_PIL_EVENT_STATE);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	TEST_EXPECT(true, isCalled);
}
