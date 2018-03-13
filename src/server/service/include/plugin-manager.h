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
 * @file        plugin-manager.h
 * @author      Sangwan Kwon(sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Plugin manager for binding backend.
 */
#pragma once

#include <string>
#include <memory>

#include <plugin-loader.h>

namespace AuthPasswd {

enum class BackendType {
	SW,
	TZ
};

class PluginManager final {
public:
	explicit PluginManager() noexcept = default;
	~PluginManager() = default;

	PluginManager(const PluginManager&) = delete;
	PluginManager& operator=(const PluginManager&) = delete;

	PluginManager(PluginManager&&) = delete;
	PluginManager& operator=(PluginManager&&) = delete;

	bool isSupport(BackendType type) const noexcept;

	void setBackend(BackendType type);

	template<typename T>
	void loadFactory(const std::string& name, T& factory);

private:
	std::string getPluginPath(BackendType type) const noexcept;

	std::shared_ptr<PluginLoader> m_loader = nullptr;
};

template<typename T>
void PluginManager::loadFactory(const std::string& name, T& factory)
{
	m_loader->load(name, factory);
}

} //namespace AuthPasswd
