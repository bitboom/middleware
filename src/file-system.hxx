/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        file-system.hxx
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#pragma once

#include <string>
#include <memory>

namespace tanchor {

using FilePtr = std::unique_ptr<FILE, decltype(&::fclose)>;

class File {
public:
	static void linkTo(const std::string &src, const std::string &dst);
	static std::string readLink(const std::string &path);
	static std::string getName(const std::string &path);
	static std::string read(const std::string &path);
};

} // namespace tanchor
