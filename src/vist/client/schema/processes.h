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


/**
 * @file processes.h
 * @brief The scheme of processes (sync with osquery/tables/spec/x/processes.table)
 */


#pragma once

#include <string>

namespace vist {
namespace schema {

struct Processes {
	long long int pid;
	std::string name;
	std::string path;
	std::string cmdline;
	long long int uid;
	long long int gid;
	long long int euid;
	long long int egid;
	int on_disk;
	long long int resident_size;
	long long int parent;
};

} // namesapce schema
} // namesapce vist
