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
/*
 * @brief Result<Valud, ErrCode> type contains either value or error_code.
 *        This is inspired by rust's syntax(std::result::Result).
 *        (ref: https://doc.rust-lang.org/std/result/enum.Result.html)
 * @usage
 *  enum class FileErr {
 *    InvalidArgument = 1,
 *    NotFound
 *  };
 *
 *  Result<std::string, FileErr> read(const std::string& path)
 *  {
 *    if (path.empty())
 *      return FileErr::InvalidArgument;
 *
 *    // Read process
 *    return "File contents";
 *  }
 *
 *  auto result = read("foo.txt");
 *  if (result.ok())
 *    std::string content = result.get();
 *  else
 *    FileErr ec = result.getErrCode();
 */

#pragma once

#include <type_traits>
#include <utility>

namespace vist {

template <typename Value, typename ErrCode>
class Result final {
public:
	static_assert(std::is_enum<ErrCode>::value, "Error code must be enum type.");
	static_assert(!std::is_reference<Value>::value,
				  "Value type must not be reference.");

	Result(Value value) : value(std::move(value)), isError(false) {}
	Result(ErrCode ec) : ec(std::move(ec)), isError(true) {}
	Result() = delete;

	inline bool ok()
	{
		return !isError;
	}
	inline bool err()
	{
		return isError;
	}

	/// TBD: Consider to return rvalue.
	inline Value& get()
	{
		return value;
	}
	inline ErrCode getErrCode()
	{
		return ec;
	}

private:
	Value value;
	ErrCode ec = static_cast<ErrCode>(0);
	bool isError = false;
};

} // namespace vist
