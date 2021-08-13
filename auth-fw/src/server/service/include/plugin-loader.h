/*
 *  Copyright (c) 2018 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        plugin-loader.h
 * @author      Sangwan Kwon(sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Plugin loader for loading backend.
 */
#pragma once

#include <string>
#include <stdexcept>

#include <dlfcn.h>

namespace AuthPasswd {

class PluginLoader final {
public:
	explicit PluginLoader(const std::string& path, int flag = RTLD_LAZY);
	~PluginLoader();

	PluginLoader(const PluginLoader&) = delete;
	PluginLoader& operator=(const PluginLoader&) = delete;

	PluginLoader(PluginLoader&&) = delete;
	PluginLoader& operator=(PluginLoader&&) = delete;

	template<typename T>
	void load(const std::string& name, T& symbol);

private:
	void* m_handle;
};

template<typename T>
void PluginLoader::load(const std::string& name, T& symbol)
{
	symbol = reinterpret_cast<T>(::dlsym(m_handle, name.c_str()));
	if (symbol == nullptr)
		throw std::runtime_error("Failed to load: " + name);
}

} //namespace AuthPasswd
