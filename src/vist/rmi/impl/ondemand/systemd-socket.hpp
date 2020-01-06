/*
 *  Copyright (c) 2020 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/logger.hpp>
#include <vist/exception.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <systemd/sd-daemon.h>

namespace vist {
namespace rmi {
namespace impl {
namespace ondemand {

class SystemdSocket {
public:
	static int Create(const std::string& path)
	{
		static int fds = -1;

		if (fds == -1)
			fds = ::sd_listen_fds(0);

		if (fds < 0)
			THROW(ErrCode::RuntimeError) << "Failed to get listened systemd fds.";

		for (int fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + fds; ++fd) {
			if (::sd_is_socket_unix(fd, SOCK_STREAM, 1, path.c_str(), 0) > 0) {
				INFO(VIST) << "Systemd socket of service is found with fd: " << fd;
				return fd;
			}
		}

		THROW(ErrCode::RuntimeError) << "Failed to find listened systemd fds.";
	}
};

} // namespace ondemand
} // namespace impl
} // namespace rmi
} // namespace vist
