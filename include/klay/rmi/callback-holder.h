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

#ifndef __RMI_CALLBACK_HOLDER_H__
#define __RMI_CALLBACK_HOLDER_H__

#include <utility>
#include <functional>

#include <klay/rmi/message.h>

namespace rmi {

template<typename Type, typename... Args>
struct MethodHandler {
	typedef std::function<Type(Args&...)> type;
};
// using MethodHandler = std::function<Type(Args&...)>;

template<typename Type, typename... Args>
struct CallbackHolder {
	template<int...>
	struct Sequence {};

	template<int N, int... S>
	struct SequenceExpansion : SequenceExpansion<N-1, N-1, S...> {};

	template<int... S>
	struct SequenceExpansion<0, S...> {
		typedef Sequence<S...> type;
	};

	std::tuple<Args...> parameters;
	const typename MethodHandler<Type, Args...>::type& callback;

	CallbackHolder(const typename MethodHandler<Type, Args...>::type& m) :
		callback(m)
	{
	}

	Type dispatch(Message& message)
	{
		return callCallback(message, typename SequenceExpansion<sizeof...(Args)>::type());
	}

	template<typename...R>
	Type callCallback(Message& message, R&... args)
	{
		message.unpackParameters(args...);
		return callback(args...);
	}

	template<int... S>
	Type callCallback(Message& message, Sequence<S...>)
	{
		return callCallback(message, std::get<S>(parameters)...);
	}
};

} // namespace rmi
#endif //!__RMI_CALLBACK_HOLDER_H__
