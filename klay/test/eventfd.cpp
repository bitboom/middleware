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

#include <string>
#include <vector>

#include <klay/error.h>
#include <klay/eventfd.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>

#include <klay/testbench.h>

TESTCASE(EventFdHandleNegative)
{
	try {
		klay::EventFD evtfd(0, -1);
	} catch (klay::Exception& e) {
	}
}

TESTCASE(EventFdSendPositive)
{
	try {
		klay::EventFD evtfd;
		evtfd.send();
		evtfd.receive();
	} catch (klay::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(EventFdSendNegative)
{
	try {
		klay::EventFD evtfd;
		evtfd.close();
		evtfd.send();
	} catch (klay::Exception& e) {
	}
}

TESTCASE(EventFdReceiveNegative)
{
	try {
		klay::EventFD evtfd;
		evtfd.close();
		evtfd.receive();
	} catch (klay::Exception& e) {
	}
}


