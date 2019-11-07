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

#include <tuple>

namespace vist {
namespace tsqb {
namespace tuple_helper {
namespace internal {

template<int n, typename T, typename C>
class Iterator {
public:
	Iterator(const T& tuple, C&& closure) {
		Iterator<n - 1, T, C> iter(tuple, std::forward<C>(closure));
		closure(std::get<n-1>(tuple));
	}
};

template<typename T, typename C>
class Iterator<0, T, C> {
public:
	Iterator(const T&, C&&) {}
};

} // namespace internal

template<typename T, typename C>
void for_each(const T& tuple, C&& closure)
{
	using Iter = internal::Iterator<std::tuple_size<T>::value, T, C>;
	Iter iter(tuple, std::forward<C>(closure));
}

} // namspace tuple-hepler
} // namspace tsqb
} // namspace vist
