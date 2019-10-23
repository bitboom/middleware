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

#include "query.h"

#include "ipc/client.h" 

namespace {
	const std::string SOCK_ADDR = "/tmp/.vist";
} // anonymous namespace

namespace vist {

Rows Query::Execute(const std::string& statement)
{
	auto& client = ipc::Client::Instance(SOCK_ADDR);

	return client->methodCall<Rows>("Vist::query", statement);
}

} // namespace vist
