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

#ifndef __DPM_PREFERENCES_H__
#define __DPM_PREFERENCES_H__

#include <string>
#include <unordered_map>

#include <klay/reflection.h>

class PreferencesStore {
public:
	typedef std::unordered_map<std::string, std::string> type;

	PreferencesStore(const std::string& pathname);
	std::string get(const std::string& key);
	void load(const std::string& filename);

	static PreferencesStore& getInstance();

private:
	PreferencesStore();

private:
	std::unordered_map<std::string, std::string> keyValueMap;
};

#endif //__DPM_PREFERENCES_H__
