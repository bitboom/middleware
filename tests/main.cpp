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
 * @file        main.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implemetation of testbench driver
 */

#include <klay/testbench.h>

#include <curl/curl.h>

int main(int /*argc*/, char** /*argv*/)
{
#ifdef TIZEN_TEST_GCOV
	::setenv("GCOV_PREFIX", "/tmp", 1);
#endif

	curl_global_init(CURL_GLOBAL_DEFAULT);

	testbench::Testbench::runAllTestSuites();

	curl_global_cleanup();

	return 0;
}
