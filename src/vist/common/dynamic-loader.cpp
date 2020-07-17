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

#include <vist/dynamic-loader.hpp>

namespace vist {

DynamicLoader::DynamicLoader(const std::string& path, int flag)
	: scopedHandle(init(path, flag))
{
}

DynamicLoader::ScopedHandle DynamicLoader::init(const std::string& path, int flag)
{
	auto open = [&]() -> void* {
		::dlerror();

		auto handle = ::dlopen(path.c_str(), flag);
		if (handle == nullptr) {
			if (auto error = ::dlerror(); error != nullptr)
				ERROR(VIST) << "Failed to open library: " << error;

			THROW(ErrCode::RuntimeError) << "Failed to open library: " << path;
		}

		return handle;
	};

	auto close = [&](void* /*handle*/) {
		::dlerror();

// Calling symbol & code after dlclose() makes SEGFAULT.
// TODO: Sync dynamic loading's life-cycle with program.
//
//		if (::dlclose(handle) != 0)
//			THROW(ErrCode::RuntimeError) << "Failed to close library: " << ::dlerror();
	};

	return ScopedHandle(open(), close);
}

} // namespace vist
