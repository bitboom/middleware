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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vist {

using Row = std::map<std::string, std::string>;
using Rows = std::vector<Row>;

class Vistd final : public std::enable_shared_from_this<Vistd> {
public:
	Vistd(const Vistd&) = delete;
	Vistd& operator=(const Vistd&) = delete;

	Vistd(Vistd&&) = default;
	Vistd& operator=(Vistd&&) = default;

	/// Exposed method (API)
	Rows query(const std::string& statement);

	static Vistd& Instance()
	{
		static Vistd instance;
		return instance;
	}

	static Rows Query(const std::string& statement)
	{
		return Vistd::Instance().query(statement);
	}

	static void Start()
	{
		Vistd::Instance().start();
	}

private:
	explicit Vistd();
	~Vistd() = default;

	void start();
};

} // namespace vist
