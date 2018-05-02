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

#include "syspopup.h"
#include <klay/gmainloop.h>
#include <klay/audit/logger.h>
#include <klay/audit/dlog-sink.h>
#include <glib.h>
#include <unistd.h>
#include <thread>

namespace {

std::unique_ptr<audit::DlogLogSink> _sink = nullptr;

} // namespace

audit::LogSink *SINK = nullptr;

class SyspopupMainLoop {
public:
	SyspopupMainLoop() :
		mainloop(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
	{
		handle = std::thread(g_main_loop_run, mainloop.get());
	}
	~SyspopupMainLoop()
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

int main()
{
	_sink.reset(new audit::DlogLogSink("DPM"));
	SINK = dynamic_cast<audit::LogSink*>((_sink).get());

	try {
		SyspopupMainLoop mainloop;
		SyspopupService syspopup;
		syspopup.run();

		::sleep(3);
	} catch (std::exception &e) {
		ERROR(SINK, e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
