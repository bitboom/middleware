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

#include <klay/testbench.h>

#include <iostream>
#include <string>

#include <getopt.h>

namespace klay {
namespace testbench {

namespace {

void usage(const std::string name)
{
	std::cout << "Usage: " << name << " [Option]" << std::endl
			  << std::endl
			  << "Options :" << std::endl
			  << "   -a, --run-all               run all TESTCASES" << std::endl
			  << "   -r, --run=[TESTCASE]        run TESTCASE" << std::endl
			  << "   -l, --list                  list TESTCASES" << std::endl
			  << "   -h, --help                  show this" << std::endl
			  << std::endl;
}

} // anonymous namespace

void Testbench::run(int argc, char* argv[])
{
	struct option options[] = {
		{"run-all", no_argument, 0, 'a'},
		{"run", required_argument, 0, 'r'},
		{"list", no_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	if (argc <= 1) {
		usage(argv[0]);
		TestDriver::GetInstance().run();
		return;
	}

	while (int opt = getopt_long(argc, argv, "ar:lh", options, 0)) {
		if (opt == -1)
			break;

		switch (opt) {
		case 'a':
			TestDriver::GetInstance().run();
			break;
		case 'r':
			TestDriver::GetInstance().run(optarg);
			break;
		case 'l':
			TestDriver::GetInstance().list();
			break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}
}

void Testbench::runAllTestSuites()
{
	TestDriver::GetInstance().run();
}

} //namespace testbench
} //namespace klay
