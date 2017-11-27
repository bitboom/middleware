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

#ifndef __RUNTIME_CGROUP_H__
#define __RUNTIME_CGROUP_H__

#include <string>
#include <vector>

#include <klay/klay.h>

namespace runtime {

class KLAY_EXPORT Cgroup final {
public:
	Cgroup() = delete;

	static bool existSubsystem(const std::string& name);
	static void createSubsystem(const std::string& name);
	static void destroySubsystem(const std::string& name);

	static bool exist(const std::string& subsystem, const std::string& path);
	static void create(const std::string& subsystem, const std::string& path);
	static void destroy(const std::string& subsystem, const std::string& path);

	static void addProcess(const std::string& subsystem,
							const std::string& path, const pid_t pid);
	static std::vector<pid_t> getProcessList(const std::string& subsystem,
											const std::string& path);

	static const std::string getPath(const std::string& subsystem, const pid_t pid);
};

} // namespace runtime

#endif //!__RUNTIME_CGROUP_H__
