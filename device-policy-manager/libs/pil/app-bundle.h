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

#ifndef __DPM_BUNDLE_H__
#define __DPM_BUNDLE_H__

#include <string>
#include <vector>

#include <bundle.h>

class Bundle {
public:
	Bundle();
	~Bundle();

	template<typename T>
	void add(const std::string& key, const std::vector<T>& value)
	{
		addArrayInternal(key, value);
	}

	template<typename T>
	void add(const std::string& key, const T& value)
	{
		addInternal(key, value);
	}

	bundle* get() const
	{
		return handle;
	}

private:
	void addInternal(const std::string& key, const std::string& value);
	void addArrayInternal(const std::string& key, const std::vector<std::string>& array);

private:
	bundle* handle;
};

#endif //__DPM_BUNDLE_H__
