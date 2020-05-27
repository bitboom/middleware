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

#include "policy-loader.hpp"

namespace vist {
namespace policy {

PolicyProvider* PolicyLoader::load(const std::string& path)
{
	PluginLoader loader(path);
	PolicyProvider::FactoryType factory = nullptr;
	loader.load(PolicyProvider::getFactoryName(), factory);
	if (factory == nullptr)
		THROW(ErrCode::RuntimeError) << "Failed to load factory: "
									 << PolicyProvider::getFactoryName();

	auto provider = (*factory)();
	if (provider == nullptr)
		THROW(ErrCode::RuntimeError) << "Failed to make provider: "
									 << PolicyProvider::getFactoryName();

	return provider;
}

PluginLoader::PluginLoader(const std::string& path, int flag)
	: handle(::dlopen(path.c_str(), flag), [](void*)->int{return 0;})
	  // Cleaning object after dlclose() makes SEGFAULT.
	  // TODO: Sync dynamic loading's life-cycle with program.(PluginManager)
	  //  : handle(::dlopen(path.c_str(), flag), ::dlclose)
{
	if (handle == nullptr)
		THROW(ErrCode::LogicError) << "Failed to open: " << path;
}

} // namespace policy
} // namespace vist
