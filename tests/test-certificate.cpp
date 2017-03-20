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
 * @file        test-certificate.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Unit test program of Certificate
 */

#include <klay/testbench.h>

#include <iostream>
#include <stdexcept>

#include "certificate.hxx"
#include "test-resource.hxx"

TESTCASE(GET_SUBJECT_NAME_HASH)
{
	try {
		tanchor::Certificate certificate(TEST_PEM_PATH);
		auto hash = certificate.getSubjectNameHash();
		TEST_EXPECT(true, hash.compare(TEST_PEM_HASH) == 0);
	} catch (const std::exception &e) {
		std::cout << "std::exception occured." << e.what() << std::endl;
	} catch (...) {
		std::cout << "Unknown exception occured." << std::endl;
	}
}

TESTCASE(GET_CERTIFICATE_DATA)
{
	try {
		tanchor::Certificate certificate(TEST_PEM_PATH);
		auto data = certificate.getCertificateData();
		TEST_EXPECT(true, data.compare(TEST_PEM_DATA) == 0);
	} catch (const std::exception &e) {
		std::cout << "std::exception occured." << e.what() << std::endl;
	} catch (...) {
		std::cout << "Unknown exception occured." << std::endl;
	}
}
