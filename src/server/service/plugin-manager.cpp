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
 * @file        plugin-manager.cpp
 * @author      Sangwan Kwon(sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implementation of plugin manager.
 */
#include <plugin-manager.h>

#include <fstream>

namespace AuthPasswd {

bool PluginManager::isSupport(BackendType type) const noexcept
{
	if (std::ifstream(this->getPluginPath(type)))
		return true;
	else
		return false;
}

void PluginManager::setBackend(BackendType type)
{
	m_loader = std::make_shared<PluginLoader>(this->getPluginPath(type));
}

std::string PluginManager::getPluginPath(BackendType type) const noexcept
{
	if (type == BackendType::SW)
		return PLUGIN_SW_BACKEND_PATH;
	else
		return PLUGIN_TZ_BACKEND_PATH;
}

} //namespace AuthPasswd
