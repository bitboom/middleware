/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __RUNTIME_REFLECTION_H__
#define __RUNTIME_REFLECTION_H__

#include <type_traits>

#include <klay/klay.h>
#include <klay/preprocessor.h>

#define VISIT_ELEMENT(elem) v.visit(#elem, elem)

#define REFLECTABLE(...)                            \
template<typename V>                                \
void accept(V v)                                    \
{                                                   \
	FOR_EACH_VAR_ARGS(VISIT_ELEMENT, __VA_ARGS__);  \
}                                                   \
template<typename V>                                \
void accept(V v) const                              \
{                                                   \
	FOR_EACH_VAR_ARGS(VISIT_ELEMENT, __VA_ARGS__);  \
}

#define NO_REFLECTABLE_PROPERTY \
	template<typename V>        \
	static void accept(V) {}

template<typename T>
struct KLAY_EXPORT ReflectionTraitChecker {
	struct Visitor {};

	template <typename C> static std::true_type
	test(decltype(std::declval<C>().template accept(Visitor()))*);

	template <typename C> static std::false_type
	test(...);

	static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};

template<typename T>
struct KLAY_EXPORT IsReflectable : public std::integral_constant<bool, ReflectionTraitChecker<T>::value> {};

#endif //!__RUNTIME_REFLECTION_H__
