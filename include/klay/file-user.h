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

#ifndef __FILE_USER_H__
#define __FILE_USER_H__

#include <string>
#include <vector>

namespace runtime {

class FileUser final {
public:
	FileUser() = delete;

	static bool isUsedAsFD(const std::string &filePath, const pid_t pid, bool isMount = false);
	static bool isUsedAsMap(const std::string &filePath, const pid_t pid, bool isMount = false);
	static bool isUsedAsCwd(const std::string &filePath, const pid_t pid, bool isMount = false);
	static bool isUsedAsRoot(const std::string &filePath, const pid_t pid, bool isMount = false);

	static std::vector<pid_t> getList(const std::string &path, bool isMount = false);
};

} // namespace runtime

#endif /* __FILE_USER_H__ */
