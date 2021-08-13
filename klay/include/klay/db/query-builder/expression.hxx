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

#pragma once

#include "column.hxx"
#include "type.hxx"
#include "condition.hxx"

#include <type_traits>

namespace qxx {

template<typename Type>
struct Expression {
	Type value;
};

template<typename O, typename F>
Expression<Column<O, F>> expr(F O::*field)
{
	Column<O, F> anonymous = {"anonymous", field};
	return {anonymous};
}

template<typename L, typename R>
struct Lesser : public condition::Binary<L, R> {
	using condition::Binary<L, R>::Binary;

	operator std::string() const
	{
		return "<";
	}
};

template<typename L, typename R>
Lesser<L, R> operator<(Expression<L> expr, R r)
{
	return {expr.value, r};
}

template<typename L, typename R>
struct Equal : public condition::Binary<L, R>
{
	using condition::Binary<L, R>::Binary;

	operator std::string() const
	{
		return "=";
	}
};

template<typename L, typename R>
Equal<L, R> operator==(Expression<L> expr, R r)
{
	return {expr.value, r};
}

namespace join {

template<typename L, typename R>
struct Equal
{
	L l;
	R r;

	operator std::string() const
	{
		return "=";
	}
};

} // namespace join

template<typename L, typename R>
join::Equal<L, R> operator==(Expression<L> l, Expression<R> r)
{
	return {l.value, r.value};
}

template<typename L, typename R>
struct Greater : public condition::Binary<L, R>
{
	using condition::Binary<L, R>::Binary;

	operator std::string() const
	{
		return ">";
	}
};

template<typename L, typename R>
Greater<L, R> operator>(Expression<L> expr, R r)
{
	return {expr.value, r};
}

template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using is_condition = typename std::is_base_of<condition::Base, T>;

template<typename L,
		 typename R,
		 typename = typename qxx::enable_if_t<is_condition<L>::value
							 && qxx::is_condition<R>::value>>
condition::And<L, R> operator&&(const L& l, const R& r)
{
	return {l, r};
}

template<typename L,
		 typename R,
		 typename = typename qxx::enable_if_t<is_condition<L>::value
							 && qxx::is_condition<R>::value>>
condition::Or<L, R> operator||(const L& l, const R& r)
{
	return {l, r};
}

} // namespace qxx
