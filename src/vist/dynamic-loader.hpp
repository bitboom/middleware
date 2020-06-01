/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/exception.hpp>
#include <vist/logger.hpp>

#include <functional>
#include <memory>
#include <string>

#include <dlfcn.h>

namespace vist {

class DynamicLoader final {
public:
	explicit DynamicLoader(const std::string& path, int flag = RTLD_LAZY);

	template<typename Symbol>
	Symbol load(const std::string& name);

private:
	using ScopedHandle = std::unique_ptr<void, std::function<void(void*)>>;
	ScopedHandle init(const std::string& path, int flag);

	ScopedHandle scopedHandle;
};

template<typename T>
T DynamicLoader::load(const std::string& name)
{
	::dlerror();

	auto symbol = ::dlsym(this->scopedHandle.get(), name.c_str());
	if (symbol == nullptr) {
		if (auto error = ::dlerror(); error != nullptr)
			ERROR(VIST) << "Failed to load symbol: " << error;

		THROW(ErrCode::RuntimeError) << "Failed to load symbol: " << name;
	}

	return reinterpret_cast<T>(symbol);
}

} // namespace vist
