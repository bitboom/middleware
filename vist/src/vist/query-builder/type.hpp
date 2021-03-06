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

#include <type_traits>

namespace vist {
namespace tsqb {
namespace type {

template<typename L, typename R>
bool cast_compare(L l, R r)
{
	return l == reinterpret_cast<L>(r);
}

template<typename L, typename R>
bool compare(L, R)
{
	return std::is_same<L, R>::value;
}

template<typename L, typename R>
void assert_compare(L, R)
{
	static_assert(std::is_same<L, R>::value, "Type is unsafe.");
}

} // namespace type
} // namespace tsqb
} // namespace vist
