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
/*
 * @file   timer.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Simple timer.
 */

#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace vist {

struct Timer {
	using Predicate = std::function<bool()>;
	using Task = std::function<void()>;

	/// Execute task() once until predicate() is true.
	static void ExecOnce(Task task, Predicate predicate, unsigned int seconds)
	{
		auto worker = std::thread([task, predicate, seconds]() {
			while (1) {
				std::this_thread::sleep_for(std::chrono::seconds(seconds));

				if (predicate()) {
					task();
					break;
				}
			}
		});

		if (worker.joinable())
			worker.join();
	}
};

} // namespace vist
