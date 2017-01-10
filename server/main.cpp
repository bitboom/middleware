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
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <klay/audit/logger.h>
#include <klay/audit/dlog-sink.h>

#include "preference.h"
#include "syspopup.h"
#include "server.h"

void signalHandler(int signum)
{
	exit(0);
}

int main(int argc, char *argv[])
{
	::signal(SIGINT, signalHandler);

	::umask(022);

	try {
		audit::Logger::setBackend(new audit::DlogLogSink());
		audit::Logger::setTag("DPM");

		PreferencesStore& instance = PreferencesStore::getInstance();
		instance.load("/etc/device-policy-manager.ini");

		SyspopupService syspopup;

		Server server;
		server.run();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
