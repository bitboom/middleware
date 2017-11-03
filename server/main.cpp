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
#include <memory>

#include <klay/gmainloop.h>

#include "pil/logger.h"

#include "server.h"

void signalHandler(int signum)
{
	ERROR(DPM, "Interrupted");
	exit(0);
}

int main(int argc, char *argv[])
{
	::signal(SIGINT, signalHandler);

	try {
		ScopedGMainLoop gmainloop;
		DevicePolicyManager manager;

		manager.loadManagedClients();
		manager.loadPolicyPlugins();

		manager.run();
	} catch (std::exception &e) {
		ERROR(DPM, e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
