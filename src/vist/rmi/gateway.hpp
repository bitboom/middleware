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
 * @file   gateway.hpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @author Jaemin Ryu (jm77.ryu@samsung.com)
 * @brief  Server-side stub for exposing method. 
 */

#pragma once

#define EXPOSE(gateway, object, function) gateway.expose(object, #function, function)

#include <vist/klass/functor.hpp>
#include <vist/timer.hpp>

#include <memory>
#include <string>

namespace vist {
namespace rmi {

class Gateway final {
public:
	explicit Gateway(const std::string& path);
	~Gateway();

	Gateway(const Gateway&) = delete;
	Gateway& operator=(const Gateway&) = delete;

	Gateway(Gateway&&) = default;
	Gateway& operator=(Gateway&&) = default;

	void start(unsigned int timeout = 0, Timer::Predicate condition = nullptr);
	void stop(void);

	template<typename O, typename F>
	void expose(O& object, const std::string& name, F&& func);

private:
	class Impl;

	klass::FunctorMap functorMap;
	std::unique_ptr<Impl> pImpl;
};

template<typename O, typename F>
void Gateway::expose(O& object, const std::string& name, F&& func)
{
	auto functor = klass::make_functor_ptr(object, std::forward<F>(func));
	this->functorMap[name] = functor;
}

} // namespace rmi
} // namespace vist
