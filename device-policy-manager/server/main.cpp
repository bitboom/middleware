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

namespace {

void usage(const std::string& prog)
{
	std::cout << "Usage: " << prog << "\n"
			  << "Options: \n"
			  << "  -o --on-demand timeout : Start as on-demand service\n"
			  << "                           Service will be unloaded after timeout\n"
			  << std::endl;
}

void signalHandler(int signum)
{
	ERROR(DPM, "Interrupted");
	exit(0);
}

} // namespace
int main(int argc, char *argv[])
{
	struct option options[] = {
		{"on-demand", required_argument, 0, 'o'},
		{"help",      no_argument,       0, 'h'},
		{0,           0,                 0,  0 }
	};

	::signal(SIGINT, signalHandler);

	bool ondemand = false;
	int index, opt, timeout = -1;
	while ((opt = getopt_long(argc, argv, "o:h", options, &index)) != -1) {
		switch (opt) {
		case 'o':
			ondemand = true;
			timeout = ::atoi(optarg);
			break;
		case 'h':
			usage(argv[0]);
			return EXIT_SUCCESS;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	::umask(0);

	try {
		ScopedGMainLoop gmainloop;
		DevicePolicyManager manager;

		manager.loadPolicyPlugins();
		manager.applyPolicies();

		manager.run(ondemand, timeout);
	} catch (std::exception &e) {
		ERROR(DPM, e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
