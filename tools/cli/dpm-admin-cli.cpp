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

#include <dpm/administration.h>

static void printUsage()
{
	printf("Usage: dpm-admin-cli [option] [app id] -u [user name]\n");
	printf("Options:\n");
	printf("  -r --register appid   : Register device admin client package\n");
	printf("  -d --deregister appid : Deregister device admin client package\n");
	printf("  -u --user username    : Specify user name that the device admin package is associted\n");
	printf("  -h --help             : print usage\n");
	printf("\n");
	printf("Note:\n");
	printf("  To register/deregister device admin client, you should specify user name that the device admin client is associated.\n");
	printf("  Ex.: dpm-admin-cli -r org.tizen.dpm -u owner\n");
	printf("\n");
}

static int registAdminClient(const char* pkgName, uid_t uid)
{
	if (pkgName == NULL)
		return -1;

	int ret;
	device_policy_manager_h handle;

	handle = dpm_manager_create();
	if (handle == NULL) {
		printf("Failed to create client handle\n");
		return -1;
	}

	ret = 0;
	if (dpm_admin_register_client(handle, pkgName, uid) != DPM_ERROR_NONE) {
		printf("Failed to register admin client. (name:%s, uid:%d)\n", pkgName, uid);
		ret = -1;
	}

	dpm_manager_destroy(handle);

	return ret;
}

static int deregistAdminClient(const char* pkgName, uid_t uid)
{
	if (pkgName == NULL)
		return -1;

	int ret;
	device_policy_manager_h handle;

	handle = dpm_manager_create();
	if (handle == NULL) {
		printf("Failed to create client handle\n");
		return -1;
	}

	ret = 0;
	if (dpm_admin_deregister_client(handle, pkgName, uid) != DPM_ERROR_NONE) {
		printf("Failed to deregister admin client. (name:%s, uid:%d)\n", pkgName, uid);
		ret = -1;
	}

	dpm_manager_destroy(handle);

	return ret;
}

static int getUID(const char* userName)
{
	struct passwd pwd, *result;
	int bufsize;

	if (userName == NULL)
		return -1;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;

	std::unique_ptr<char[]> buf(new char[bufsize]);
	::getpwnam_r(userName, &pwd, buf.get(), bufsize, &result);
	if (result == NULL) {
		printf("User %s doesn't exist\n", userName);
		return -1;
	}

	return (int)result->pw_uid;
}

enum OptVal {
	DPM_ADMIN_CLI_REGISTER,
	DPM_ADMIN_CLI_DEREGISTER,
	DPM_ADMIN_CLI_NULL,
};

int main(int argc, char *argv[])
{
	int uid = 0;
	char* userName = NULL;
	char* pkgName = NULL;

	int opt = 0;
	OptVal optVal = DPM_ADMIN_CLI_NULL;
	struct option options[] = {
		{"register",   required_argument, 0, 'r'},
		{"deregister", required_argument, 0, 'd'},
		{"user",       required_argument, 0, 'u'},
		{"help",       no_argument,       0, 'h'},
		{0,            0,                 0,  0 }
	};

	int index = 0;
	while ((opt = getopt_long(argc, argv, "r:d:u:h", options, &index)) != -1) {
		switch(opt) {
		case 'r':
			optVal = DPM_ADMIN_CLI_REGISTER;
			pkgName = optarg;
			break;
		case 'd':
			optVal = DPM_ADMIN_CLI_DEREGISTER;
			pkgName = optarg;
			break;
		case 'u':
			userName = optarg;
			break;
		case 'h':
			printUsage();
			return 0;
		default:
			printUsage();
			exit(EXIT_FAILURE);
		}
	}

	uid = getUID(userName);
	if (uid == -1) {
		printUsage();
		exit(EXIT_FAILURE);
	}

	switch(optVal) {
	case DPM_ADMIN_CLI_REGISTER:
		return registAdminClient(pkgName, static_cast<uid_t>(uid));
	case DPM_ADMIN_CLI_DEREGISTER:
		return deregistAdminClient(pkgName, static_cast<uid_t>(uid));
	default:
		printUsage();
		exit(EXIT_FAILURE);
	}

	return 0;
}
