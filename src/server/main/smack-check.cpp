/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        smack-check.cpp
 * @author      Jooseong Lee (jooseong.lee@samsung.com)
 * @version     1.0
 * @brief       Check smck is enabled on device.
 */
#include "smack-check.h"

#include <fstream>
#include <sys/smack.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <dpl/log/log.h>
#include <error-description.h>

namespace AuthPasswd {

const char COMMENT = '#';
const std::string CLIENT_WHITELIST = "/etc/auth-fw/client-whitelist";
const std::string ADMIN_CLIENT_WHITELIST = "/etc/auth-fw/admin-client-whitelist";

int smack_runtime_check(void)
{
	static int smack_present = -1;

	if (-1 == smack_present) {
		if (NULL == smack_smackfs_path()) {
			LogDebug("no smack found on device");
			smack_present = 0;
		} else {
			LogDebug("found smack on device");
			smack_present = 1;
		}
	}

	return smack_present;
}

int smack_check(void)
{
#ifndef SMACK_ENABLED
	return 0;
#else
	return smack_runtime_check();
#endif
}

bool checkClientOnWhitelist(int sockfd, std::string whitelistPath)
{
	struct ucred cr;
	socklen_t len = sizeof(struct ucred);

	// get client smack label from socket
	if (getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &cr, &len)) {
		int err = errno;
		LogError("getsockopt() failed: " << errnoToString(err));
		return false;
	}

	std::string clientSmackLabel;
	std::string path("/proc/" + std::to_string(cr.pid) + "/attr/current");
	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		LogError("failed to open " << path);
		return false;
	}

	std::getline(file, clientSmackLabel);
	file.close();
	if (clientSmackLabel.empty())
		return false;

	// compare with whitelist labels
	std::string line;
	std::ifstream whitelistFile(whitelistPath.c_str());
	if (!whitelistFile.is_open()) {
		LogError("failed to open " << whitelistPath);
		return false;
	}

	while (std::getline(whitelistFile, line)) {
		if (line.empty())
			continue;
		if (line.at(0) == COMMENT)
			continue;
		if (line.compare(clientSmackLabel) == 0) {
			whitelistFile.close();
			LogDebug("Client " << clientSmackLabel << " is on whitelist");
			return true;
		}
	}
	whitelistFile.close();
	LogError("Client " << clientSmackLabel << " is not on whitelist");

	return false;
}

} // namespace AuthPasswd
