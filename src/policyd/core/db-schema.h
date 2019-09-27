/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <string>

namespace policyd {
namespace schema {

struct Admin {
	int id = -1;
	std::string pkg;
	int uid = -1;
	std::string key;
	int removable = -1;
};

struct ManagedPolicy {
	int id = -1;
	int aid = -1;
	int pid = -1;
	int value = -1;
};

struct PolicyDefinition {
	int id = -1;
	int scope = -1;
	std::string name;
	int ivalue = -1;
};

} // namespace schema
} // namespace policyd
