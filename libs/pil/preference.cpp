/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <fstream>
#include <algorithm>

#include <klay/exception.h>
#include <klay/audit/logger.h>

#include "preference.h"

namespace {

std::string& trimLeadingWhiteSpaces(std::string& s)
{
	size_t startpos = s.find_first_not_of(" \t\r\n\v\f");
	if (std::string::npos != startpos) {
		s = s.substr(startpos);
	}

	return s;
}

std::string& trimTrailingWhiteSpaces(std::string& s)
{
	size_t endpos = s.find_last_not_of(" \t\r\n\v\f");
	if (std::string::npos != endpos) {
		s = s.substr(0, endpos + 1);
	}

	return s;
}

std::unordered_map<std::string, std::string> defaultPreferences = {
	{"locktype", "org.tizen.setting-password"},
	{"syspopup", "org.tizen.dpm-syspopup"}
};

} // namespace

PreferencesStore::PreferencesStore()
{
}

PreferencesStore& PreferencesStore::getInstance()
{
	static PreferencesStore __instance__;
	return __instance__;
}

std::string PreferencesStore::get(const std::string& key)
{
	type::const_iterator it = keyValueMap.find(key);
	if (it != keyValueMap.end())
		return it->second;

	return "";
}

void PreferencesStore::load(const std::string& filename)
{
	std::ifstream source;
	source.open(filename);

	if (!source) {
		keyValueMap = defaultPreferences;
		return;
	}

	for (std::string line; std::getline(source, line);) {
		size_t startpos = line.find_first_not_of(" \t\r\n\v\f");
		if (startpos == std::string::npos) {
			continue;
		} else {
			line = line.substr(startpos);
		}

		switch (line[0]) {
		case ';':
		case '#':
				break;
		default:
			size_t end = line.find_first_of("=:");
			if (end == std::string::npos) {
				throw runtime::Exception("Invalid syntax");
			}
			std::string name = line.substr(0, end);
			std::string value = line.substr(end + 1);

			trimLeadingWhiteSpaces(trimTrailingWhiteSpaces(name));
			trimLeadingWhiteSpaces(trimTrailingWhiteSpaces(value));
			keyValueMap[name] = value;
		}
	}
}
