/*
 *  Copyright (c) 2018-present Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file   function.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Hold the class member fuction.
 */

#pragma once

#include <type_traits>

namespace vist {
namespace klass {

template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;
template<typename T>
using remove_ref_t = typename std::remove_reference<T>::type;
template<typename T>
using remove_cv_ref_t = remove_cv_t<remove_ref_t<T>>;

template<typename R, typename K, typename... Ps>
class Function {
public:
	using Klass = K;
	using Return = R;
	using Parameters = std::tuple<remove_cv_ref_t<Ps>...>;
	using Pointer = Return (Klass::*)(Ps...);

	auto get(void) noexcept -> const Pointer&;

private:
	explicit Function(Pointer pointer);

	template<typename RR, typename KK, typename... PPs>
	friend Function<RR, KK, PPs...> make_function(RR (KK::* member)(PPs...));

	Pointer pointer;
};

template<typename R, typename K, typename... Ps>
Function<R, K, Ps...>::Function(Pointer pointer) : pointer(pointer)
{
}

template<typename R, typename K, typename... Ps>
auto Function<R, K, Ps...>::get(void) noexcept -> const Pointer&
{
	return this->pointer;
}

template<typename R, typename K, typename... Ps>
Function<R, K, Ps...> make_function(R (K::* member)(Ps...))
{
	constexpr bool notVoid = !(std::is_same<R, void>::value);
	static_assert(notVoid, "Return type cannot be void.");

	using IsValid = std::is_member_function_pointer<decltype(member)>;
	static_assert(IsValid::value, "Pamameter should be member function type.");

	return Function<R, K, Ps...>(member);
}

} // namespace klass
} // namespace vist
