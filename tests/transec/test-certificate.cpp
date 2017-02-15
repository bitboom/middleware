/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        test-certificate.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of Certificate
 */

#include <dpl/test/test_runner.h>

#include <iostream>

#include <Certificate.h>

#include "test-resource.h"

RUNNER_TEST_GROUP_INIT(T0600_CERTIFICATE)

using namespace transec;

RUNNER_TEST(T0601_GET_SUBJECT_NAME_HASH)
{
	try {
		Certificate certificate(TEST_PEM_PATH);
		auto hash = certificate.getSubjectNameHash();
		RUNNER_ASSERT_MSG(hash.compare(TEST_PEM_HASH) == 0,
						  "Failed to get proper hash.");
	} catch (const std::exception &e) {
		std::cout << "std::exception occured." << e.what() << std::endl;
	} catch (...) {
		std::cout << "Unknown exception occured." << std::endl;
	}
}

RUNNER_TEST(T0602_GET_CERTIFICATE_DATA)
{
	try {
		Certificate certificate(TEST_PEM_PATH);
		auto data = certificate.getCertificateData();
		RUNNER_ASSERT_MSG(data.compare(TEST_PEM_DATA) == 0,
						  "Failed to get proper certificate data.");
	} catch (const std::exception &e) {
		std::cout << "std::exception occured." << e.what() << std::endl;
	} catch (...) {
		std::cout << "Unknown exception occured." << std::endl;
	}
}
