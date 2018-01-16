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
#ifndef __GMAINLOOP_WRAPPER_H__
#define __GMAINLOOP_WRAPPER_H__

#include <atomic>
#include <thread>
#include <future>
#include <memory>
#include <glib.h>
#include <deque>
#include <mutex>
#include <functional>

#include <klay/klay.h>
#include <klay/audit/logger.h>

class KLAY_EXPORT ScopedGMainLoop {
public:
	ScopedGMainLoop() :
		done(0),
		context(g_main_context_new(), g_main_context_unref),
		mainloop(g_main_loop_new(context.get(), FALSE), g_main_loop_unref)
	{
		worker = std::thread([this] {
			g_main_context_push_thread_default(context.get());
			while (!done) {

				{
					std::lock_guard<std::mutex> l(queuelock);
					if (!tasks.empty()) {
						try {
							auto&& task = tasks.front();
							task.first();

							auto&& barrier = std::move(task.second);
							barrier.set_value();

							tasks.pop_front();
						} catch (std::future_error& e) {
							ERROR(KSINK, e.what());
						}
					}
				}

				g_main_context_iteration(context.get(), TRUE);
			}

			g_main_context_pop_thread_default(context.get());
		});
	}

	~ScopedGMainLoop()
	{
		done = true;
		g_main_context_wakeup(context.get());

		if (worker.joinable())
			worker.join();
	}

	void dispatch(std::function<void(void)>&& task)
	{
		if (done)
			return;

		try {
			std::promise<void> barrier;
			std::future<void> future = barrier.get_future();

			{
				std::lock_guard<std::mutex> l(queuelock);
				tasks.emplace_back(std::make_pair(std::move(task), std::move(barrier)));
			}

			g_main_context_wakeup(context.get());
			future.wait();
		} catch (std::future_error& e) {
			ERROR(KSINK, e.what());
		}
	}

private:
	std::unique_ptr<GMainContext, void(*)(GMainContext*)> context;
	std::unique_ptr<GMainLoop, void(*)(GMainLoop*)> mainloop;

	std::atomic<bool> done;

	std::thread worker;

	std::mutex queuelock;

	using Task = std::pair<std::function<void(void)>, std::promise<void>>;
	std::deque<Task> tasks;
};
#endif
