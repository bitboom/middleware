/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/types.h>
#include <unistd.h>

#include <klay/exception.h>
#include <klay/thread-pool.h>

#define __BEGIN_CRITICAL__  { std::unique_lock<std::mutex> lock(this->queueMutex);
#define __END_CRITICAL__    }

namespace klay {

ThreadPool::ThreadPool(size_t threads)
	: stop(false)
{
	for (size_t i = 0; i < threads; i++) {
	   workers.emplace_back([this] {
			while (true) {
				std::function<void()> task;

				__BEGIN_CRITICAL__
				condition.wait(lock, [this]{ return stop || !tasks.empty();});
				if (stop && tasks.empty()) {
					return;
				}

				task = std::move(tasks.front());
				tasks.pop_front();
				__END_CRITICAL__

				task();
			}
		});
	}
}

ThreadPool::~ThreadPool()
{
	__BEGIN_CRITICAL__
	stop = true;
	__END_CRITICAL__

	condition.notify_all();

	for (std::thread &worker: workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
}

void ThreadPool::submit(std::function<void()>&& task)
{
	__BEGIN_CRITICAL__
	if (!stop) {
		tasks.push_back(std::forward<std::function<void()>>(task));
	}
	__END_CRITICAL__

	condition.notify_one();
}

} // namespace klay
