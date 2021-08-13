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

#ifndef __RUNTIME_THREAD_POOL_H__
#define __RUNTIME_THREAD_POOL_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <deque>

#include <klay/klay.h>

namespace klay {

class KLAY_EXPORT ThreadPool {
public:
	ThreadPool(size_t threads);
	~ThreadPool();

	void submit(std::function<void()>&& task);

private:
	std::vector<std::thread> workers;
	std::deque<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop;
};

} // namespace klay

namespace runtime = klay;

#endif //__RUNTIME_THREAD_POOL_H__
