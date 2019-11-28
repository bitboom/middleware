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
#include <string>
#include <vector>

namespace vist {

using Row = std::map<std::string, std::string>;
using Rows = std::vector<Row>;

class Vist final {
public:
	~Vist() = default;

	Vist(const Vist&) = delete;
	Vist& operator=(const Vist&) = delete;

	Vist(Vist&&) = default;
	Vist& operator=(Vist&&) = default;

	/// Exposed method (API)
	Rows query(const std::string& statement);

	static Vist& Instance()
	{
		static Vist instance;
		return instance;
	}

	static Rows Query(const std::string& statement)
	{
		return Vist::Instance().query(statement);
	}

	void start();

private:
	explicit Vist();
};

} // namespace vist
