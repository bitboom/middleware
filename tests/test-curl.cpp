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
/*
 * @file        test-curl.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of Curl
 */

#include <klay/testbench.h>

#include <iostream>
#include <stdexcept>

#include "test-util.hxx"
#include "test-resource.hxx"

TESTCASE(CONNECT_SSL)
{
	int ret = test::util::connectSSL("https://google.com");
	TEST_EXPECT(true, ret == 0);

	if (ret != 0)
		std::cout << "Check wifi connection.." << std::endl;
}
