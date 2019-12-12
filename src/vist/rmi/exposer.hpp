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
 * @file   exposer.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Server-side stub for exposing method. 
 */

#pragma once

#include <vist/klass/functor.hpp>

#include <memory>
#include <string>

namespace vist {
namespace rmi {

class Exposer final {
public:
	explicit Exposer(const std::string& path);

	void start(void);
	void stop(void);

	template<typename O, typename F>
	void expose(O&& object, const std::string& name, F&& func);

private:
	class Impl;
	struct ImplDeleter
	{
		void operator()(Impl*);
	};

	klass::FunctorMap functorMap;
	std::unique_ptr<Impl, ImplDeleter> pImpl;
};

template<typename O, typename F>
void Exposer::expose(O&& object, const std::string& name, F&& func)
{
	auto functor = klass::make_functor_ptr(std::forward<O>(object), std::forward<F>(func));
	this->functorMap[name] = functor;
}

} // namespace rmi
} // namespace vist
