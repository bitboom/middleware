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

#ifndef __DPM_POLICY_MANAGER_H__
#define __DPM_POLICY_MANAGER_H__

#include <string>

#include "client-manager.h"
typedef std::function<bool(const std::string&, const std::string&)> PolicyStateComparator;

bool policyAllowableComparator(const std::string& new_val, const std::string& old_val);

class PolicyManager {
public:
	PolicyManager(const std::string& path);
	~PolicyManager();

	PolicyManager(const PolicyManager&) = delete;
	PolicyManager& operator=(const PolicyManager&) = delete;

	void definePolicy(const std::string& name, const std::string& defaultVal,
				        PolicyStateComparator comparator);

	const std::string getPolicy(const std::string& name);
	void setPolicy(Client& client, const std::string& name, const std::string& state);

	void flushEffectivePolicy(const std::string& policy);

	void remove();

private:
	std::string location;

	void initialize();
};

#endif //__DPM_POLICY_MANAGER_H__
