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

#pragma once

#include <vist/policy/sdk/policy-provider.hpp>

#include <memory>
#include <stdexcept>
#include <string>

#include <dlfcn.h>

namespace vist {
namespace policy {

struct PolicyLoader final {
	static PolicyProvider* load(const std::string& path);
};

class PluginLoader final {
public:
	explicit PluginLoader(const std::string& path, int flag = RTLD_LAZY);

	template<typename T>
	void load(const std::string& name, T& symbol);

private:
	using Handle = std::unique_ptr<void, int(*)(void*)>;
	Handle handle;
};

template<typename T>
void PluginLoader::load(const std::string& name, T& symbol)
{
	symbol = reinterpret_cast<T>(::dlsym(handle.get(), name.c_str()));
	if (symbol == nullptr)
		throw std::runtime_error("Failed to load: " + name);
}

} // namespace policy
} // namespace vist
