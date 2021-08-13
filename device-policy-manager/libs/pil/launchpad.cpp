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
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include <aul.h>
#include <klay/exception.h>

#include "launchpad.h"

Launchpad::Launchpad(const uid_t uid) :
	user(uid)
{
	if (user == 0) {
		throw runtime::Exception("No logined user for launching app");
	}
}

bool Launchpad::isRunning(const std::string& appid)
{
	return ::aul_app_is_running_for_uid(appid.c_str(), user);
}

void Launchpad::launch(const std::string& appid)
{
	launch(appid, Bundle());
}

void Launchpad::launch(const std::string& appid, const Bundle& bundle)
{
	if (::aul_launch_app_for_uid(appid.c_str(), bundle.get(), user) < 0) {
		throw runtime::Exception("Failed to launch app " + appid);
	}
}

void Launchpad::resume(const std::string& appid)
{
	if (::aul_resume_app_for_uid(appid.c_str(), user) < 0) {
		throw runtime::Exception("Failed to resume app " + appid);
	}
}

void Launchpad::terminate(const std::string& appid)
{
	int pid = ::aul_app_get_pid_for_uid(appid.c_str(), user);
	if (pid > 0) {
		if (::aul_terminate_pid_for_uid(pid, user) < 0) {
			::kill(pid, SIGKILL);
		}
	}
}
