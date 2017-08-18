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
#include <klay/audit/dlog-sink.h>
#include <glib.h>
#include <unistd.h>
#include <thread>

namespace {

std::unique_ptr<audit::DlogLogSink> _sink = nullptr;

} // namespace

audit::LogSink *SINK = nullptr;

int main()
{
	_sink.reset(new audit::DlogLogSink("DPM"));
	SINK = dynamic_cast<audit::LogSink*>((_sink).get());

	ScopedGMainLoop mainloop;
	SyspopupService syspopup;

	syspopup.run();

	::sleep(3);

	return 0;
}
