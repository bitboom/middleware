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

#include <string>
#include <exception>
#include <unordered_map>
#include <memory>

#include "pil/policy-model.h"

class Plugin {
public:
	Plugin();
	virtual ~Plugin();

	int load(const std::string& path, int flags);
	void unload();
	void* lookupSymbol(const std::string& name);

	bool isLoaded() const
	{
		return handle != 0;
	}

	bool hasSymbol(const std::string& name)
	{
		return lookupSymbol(name) != 0;
	}

private:
	void* handle;
};

class PolicyLoader {
public:
	typedef AbstractPolicyProvider* (*PolicyFactory)();

	PolicyLoader(const std::string& base);
	std::shared_ptr<AbstractPolicyProvider> instantiate(const std::string& name);

private:
	typedef std::unordered_map<std::string, Plugin> PluginMap;

	std::string basename;
	PluginMap pluginMap;
};
