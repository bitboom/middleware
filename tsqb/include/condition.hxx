/*
 *  Copyright (c) 2017-present Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file condition.hxx
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Represent the condition statement of SQL
 */

#pragma once

#include "type.hxx"

namespace tsqb {
namespace condition {

struct Base {};

template<typename L, typename R>
struct And : public Base {
	L l;
	R r;

	And(L l, R r) : l(l), r(r) {}
	operator std::string() const
	{
		return "AND";
	}
};

template<typename L, typename R>
struct Or : public Base {
	L l;
	R r;

	Or(L l, R r) : l(l), r(r) {}
	operator std::string() const
	{
		return "OR";
	}
};

template<typename L, typename R>
struct Binary : public Base {
	L l;
	R r;

	Binary(L l, R r) : l(l), r(r)
	{
		using FieldType = typename L::FieldType;
		type::assert_compare(FieldType(), r);
	}
};

template<typename L>
struct Binary<L, const char*> : public Base {
	L l;
	std::string r;

	Binary(L l, const char* r) : l(l), r(r)
	{
		using FieldType = typename L::FieldType;
		type::assert_compare(FieldType(), std::string());
	}
};

enum class Join : int {
	INNER,
	CROSS,
	LEFT_OUTER,
	RIGHT_OUTER,
	FULL_OUTER
};

inline std::string to_string(Join type)
{
	switch (type) {
	case Join::CROSS: return "CROSS";
	case Join::LEFT_OUTER: return "LEFT OUTER";
	case Join::RIGHT_OUTER: return "RIGHT OUTER";
	case Join::FULL_OUTER: return "FULL OUTER";
	case Join::INNER:
	default:
		return "INNER";
	}
}

} // namespace condition
} // namespace tsqb
