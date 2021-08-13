/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test-util.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Common utilities for test.
 */
#include "test-util.h"

#include <iostream>
#include <chrono>

namespace Test {

void cmpFuncTime(const std::function<void()> &func1,
				 const std::function<void()> &func2)
{
	using namespace std::chrono;
	auto start = system_clock::now();
	func1();
	auto end = system_clock::now();
	auto mill = duration_cast<milliseconds>(end - start);
	std::cout << "[Func1] Elapsed time : "
			  << mill.count() << "ms" << std::endl;

	start = system_clock::now();
	func2();
	end = system_clock::now();
	mill = duration_cast<milliseconds>(end - start);
	std::cout << "[Func2] Elapsed time : "
			  << mill.count() << "ms" << std::endl;
}

} // namespace Test
