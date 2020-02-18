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

#pragma once

#include "column.hpp"
#include "type.hpp"
#include "util.hpp"

#include <string>
#include <type_traits>

namespace vist {
namespace tsqb {

struct Expression {};

template<typename T>
using is_expression = typename std::is_base_of<Expression, T>;

template<typename L, typename R>
struct Binary : public Expression {
	L l;
	R r;

	/// L is Column and R is Value
	Binary(L l, R r) : l(l), r(r)
	{
		/// Prevent logical expressions like &&, || and ==
		if constexpr(!is_expression<L>::value) {
			using FieldType = typename L::FieldType;
			type::assert_compare(FieldType(), r);
		}
	}
};

template<typename L>
struct Binary<L, const char*> : public Expression {
	L l;
	std::string r;

	/// Make r to 'r'
	Binary(L l, const char* r) : l(l), r("'" + std::string(r) + "'")
	{
		using FieldType = typename L::FieldType;
		type::assert_compare(FieldType(), std::string());
	}
};

template<typename L>
struct Binary<L, std::string> : public Expression {
	L l;
	std::string r;

	/// Make r to 'r'
	Binary(L l, std::string r) : l(l), r("'" + r + "'")
	{
		using FieldType = typename L::FieldType;
		type::assert_compare(FieldType(), std::string());
	}
};

template<typename L, typename R>
struct Greater : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return ">";
	}
};

template<typename L, typename R>
struct Lesser : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return "<";
	}
};

template<typename L, typename R>
struct Assign : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return "=";
	}
};

template<typename L, typename R>
struct Equal : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return "=";
	}
};

template<typename L, typename R>
struct And : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return "AND";
	}
};

template<typename L, typename R>
struct Or  : public Binary<L, R> {
	using Binary<L, R>::Binary;

	operator std::string() const
	{
		return "OR";
	}
};

template<typename L,
		 typename R,
		 typename = typename std::enable_if_t<is_expression<L>::value &&
											  is_expression<R>::value>>
And<L, R> operator&&(const L& l, const R& r)
{
	return {l, r};
}

template<typename L,
		 typename R,
		 typename = typename std::enable_if_t<is_expression<L>::value &&
											  is_expression<R>::value>>
Or<L, R> operator||(const L& l, const R& r)
{
	return {l, r};
}

} // namespace tsqb
} // namespace vist
