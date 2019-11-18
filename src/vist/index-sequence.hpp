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
 * @file   index-sequence.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief  Generate index sequence.
 */

#pragma once

#include <cstddef>

namespace vist {

struct EmptySequence {};

template<std::size_t...>
struct IndexSequence {};

namespace {

template<std::size_t N, std::size_t... S>
struct SequenceExpansion : SequenceExpansion<N-1, N-1, S...> {};

template<std::size_t... S>
struct SequenceExpansion<0, S...> { using Type = IndexSequence<S...>; };

template<>
struct SequenceExpansion<0> { using Type = EmptySequence; };

} // anonymous namespace

template<std::size_t N>
using make_index_sequence = typename SequenceExpansion<N>::Type;

} // namespace vist
