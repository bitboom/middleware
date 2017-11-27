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

#include <thread>
#include <memory>
#include <glib.h>

#include <klay/klay.h>

class KLAY_EXPORT ScopedGMainLoop {
public:
	ScopedGMainLoop() :
		mainloop(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
	{
		handle = std::thread(g_main_loop_run, mainloop.get());
	}

	~ScopedGMainLoop()
	{
		while (!g_main_loop_is_running(mainloop.get())) {
			std::this_thread::yield();
		}

		g_main_loop_quit(mainloop.get());
		handle.join();
	}

private:
	std::unique_ptr<GMainLoop, void(*)(GMainLoop*)> mainloop;
	std::thread handle;
};
#endif
