/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#pragma once

#include <vist/exception.hpp>

#include <sys/socket.h>
#include <sys/types.h>

namespace vist {

struct Credentials {
	static Credentials Peer(int fd)
	{
		struct ucred cred;
		socklen_t credsz = sizeof(cred);
		errno = 0;
		if (::getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cred, &credsz))
			THROW(ErrCode::RuntimeError) << "Failed to get peer credential: " << errno;

		return {cred.pid, cred.uid, cred.gid};
	}

	pid_t pid;
	uid_t uid;
	gid_t gid;
};

} // namespace vist
