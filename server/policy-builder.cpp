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

#include "policy-builder.h"

std::vector<std::function<void(PolicyControlContext& context)>> policyBuilder;

bool policyAllowableComparator(const std::string& old_val, const std::string& new_val)
{
	if (old_val == "allowed" && new_val == "disallowed") {
		return true;
	}
	return false;
}

bool policyEnableComparator(const std::string& old_val, const std::string& new_val)
{
	if (old_val == "enabled" && new_val == "disabled") {
		return true;
	}
	return false;
}

bool policyLastComparator(const std::string& old_val, const std::string& new_val)
{
	return true;
}

bool policyMaxComparator(const std::string& old_val, const std::string& new_val)
{
	if (old_val < new_val) {
		return true;
	}
	return false;
}

bool policyMinComparator(const std::string& old_val, const std::string& new_val)
{
	if (old_val > new_val) {
		return true;
	}
	return false;
}

