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

#pragma once

#include <vist/logger.hpp>
#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-provider.hpp>
#include <vist/sdk/policy-value.hpp>

#include <memory>
#include <string>

namespace vist {
namespace policy {

class SampleIntPolicy : public PolicyModel {
public:
	SampleIntPolicy() : PolicyModel("sample_int_policy", PolicyValue(7))
	{
	}

	void onChanged(const PolicyValue& value) override
	{
		INFO(VIST_PLUGIN) << "sample_int_policy is changed to "
						  << static_cast<int>(value);
	}
};

class SampleStrPolicy : public PolicyModel {
public:
	SampleStrPolicy() : PolicyModel("sample_str_policy", PolicyValue("TEST"))
	{
	}

	void onChanged(const PolicyValue& value) override
	{
		INFO(VIST_PLUGIN) << "sample_str_policy is changed to "
						  << static_cast<std::string>(value);
	}

	int compare(const PolicyValue& lhs, const PolicyValue& rhs) const override
	{
		std::string lvalue = lhs;
		std::string rvalue = rhs;

		INFO(VIST_PLUGIN) << "Custom compare method is called with [" << lvalue
						  << "], [" << rvalue << "]";

		return lvalue.compare(rvalue);
	}
};

class SampleProvider : public PolicyProvider {
public:
	SampleProvider(const std::string& name) : PolicyProvider(name)
	{
	}

	~SampleProvider()
	{
	}
};

} // namespace policy
} // namespace vist
