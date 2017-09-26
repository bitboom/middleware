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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <getopt.h>

#include <memory>
#include <string>

#include "status.h"
#include "policy-client.h"

static void usage()
{
	std::cout << "Usage: dpm-admin-cli [option] [app id] -u [user name]\n"
			  << "Options:\n"
			  << "  -d --deregister pkgid : Deregister device administrator\n"
			  << "  -u --user username    : Specify user name that the device administrator is associted\n"
			  << "  -h --help             : print usage\n"
			  << "\n"
			  << "Note:\n"
			  << "  To register/deregister device administrator, "
			  << "you should specify user name that the device admin client is associated.\n"
			  << "  Ex.: dpm-admin-cli -r org.tizen.dpm -u owner\n"
			  << std::endl;
}

static int registerAdministrator(const std::string& pkgname, uid_t uid)
{
	try {
		DevicePolicyClient client;

		client.connect();
		Status<int> status { -1 };
		status = client.methodCall<int>("DevicePolicyManager::enroll", pkgname, uid);
		return status.get();
	} catch (...) {
		std::cerr << "Failed to enroll device" << std::endl;
		return -1;
	}
}

static int deregisterAdministrator(const std::string& pkgname, uid_t uid)
{
	try {
		DevicePolicyClient client;

		client.connect();
		Status<int> status { -1 };
		status = client.methodCall<int>("DevicePolicyManager::disenroll", pkgname, uid);
		return status.get();
	} catch (...) {
		std::cerr << "Failed to disenroll device" << std::endl;
		return -1;
	}
}

static int getUID(const std::string& username)
{
	struct passwd pwd, *result;

	int bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);
	::getpwnam_r(username.c_str(), &pwd, buf.get(), bufsize, &result);
	if (result == NULL) {
		std::cout << "User " << username << " isn't exist" << std::endl;
		return -1;
	}

	return (int)result->pw_uid;
}

enum Command {
	NotSpecified,
	RegisterAdministrator,
	DeregisterAdministrator
};

int main(int argc, char *argv[])
{
	Command command = NotSpecified;

	struct option options[] = {
		{"register",   required_argument, 0, 'r'},
		{"deregister", required_argument, 0, 'd'},
		{"user",       required_argument, 0, 'u'},
		{"help",       no_argument,       0, 'h'},
		{0,            0,                 0,  0 }
	};

	char* username = NULL;
	char* pkgname = NULL;

	int index, opt;
	while ((opt = getopt_long(argc, argv, "r:d:u:h", options, &index)) != -1) {
		switch(opt) {
		case 'r':
			command = RegisterAdministrator;
			pkgname = optarg;
			break;
		case 'd':
			command = DeregisterAdministrator;
			pkgname = optarg;
			break;
		case 'u':
			username = optarg;
			break;
		case 'h':
			usage();
			return 0;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}

	int uid = getUID(username);
	if (uid == -1) {
		usage();
		return EXIT_FAILURE;
	}

	switch (command) {
	case RegisterAdministrator:
		return registerAdministrator(pkgname, static_cast<uid_t>(uid));
	case DeregisterAdministrator:
		return deregisterAdministrator(pkgname, static_cast<uid_t>(uid));
	default:
		usage();
	}

	return EXIT_FAILURE;
}
