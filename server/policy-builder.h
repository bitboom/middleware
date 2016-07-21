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

#ifndef __POLICY_BUILDER_H__
#define __POLICY_BUILDER_H__

#include <vector>
#include <memory>
#include <string>
#include <climits>
#include <functional>

#include "policy-context.hxx"

extern std::vector<std::function<void(PolicyControlContext& context)>> policyBuilder;

template<typename T>
struct PolicyBuilder {
	PolicyBuilder()
	{
		policyBuilder.emplace_back([this](PolicyControlContext& context) {
			instance.reset(new T(context));
		});
	}

	std::unique_ptr<T> instance;
};


bool policyAllowableComparator(const std::string& old_val, const std::string& new_val);

inline void DefineAllowablePolicy(PolicyControlContext& context, const std::string& name)
{
	context.definePolicy(name, "allowed", policyAllowableComparator);
}

inline bool IsPolicyAllowed(PolicyControlContext& context, const std::string& name)
{
	return context.getPolicy(name) == "allowed" ? true : false;
}

inline int SetPolicyAllowed(PolicyControlContext& context, const std::string& name, bool allow)
{
	return context.updatePolicy(name, allow ? "allowed" : "disallowed");
}


bool policyEnableComparator(const std::string& old_val, const std::string& new_val);

inline void DefineEnablePolicy(PolicyControlContext& context, const std::string& name)
{
	context.definePolicy(name, "enabled", policyAllowableComparator);
}

inline bool IsPolicyEnabled(PolicyControlContext& context, const std::string& name)
{
	return context.getPolicy(name) == "enabled" ? true : false;
}

inline int SetPolicyEnabled(PolicyControlContext& context, const std::string& name, bool enable)
{
	return context.updatePolicy(name, enable ? "enabled" : "disabled");
}


bool policyLastComparator(const std::string& old_val, const std::string& new_val);

inline void DefineLastPolicy(PolicyControlContext& context, const std::string& name, const std::string& initial)
{
	context.definePolicy(name, initial, policyLastComparator);
}

inline const std::string GetPolicy(PolicyControlContext& context, const std::string& name)
{
	return context.getPolicy(name);
}

inline int SetPolicy(PolicyControlContext& context, const std::string& name, const std::string& state)
{
	return context.updatePolicy(name, state);
}


bool policyMaxComparator(const std::string& old_val, const std::string& new_val);

bool policyMinComparator(const std::string& old_val, const std::string& new_val);

inline void DefineUintMaxPolicy(PolicyControlContext& context, const std::string& name, unsigned int initial = UINT_MAX)
{
	context.definePolicy(name, std::to_string(initial), policyMaxComparator);
}

inline void DefineUintMinPolicy(PolicyControlContext& context, const std::string& name, unsigned int initial = 0)
{
	context.definePolicy(name, std::to_string(initial), policyMinComparator);
}

inline unsigned int GetUintPolicy(PolicyControlContext& context, const std::string& name)
{
	return (unsigned int)std::stoul(context.getPolicy(name));
}

inline int SetUintPolicy(PolicyControlContext& context, const std::string& name, unsigned int value)
{
	return context.updatePolicy(name, std::to_string(value));
}


inline void PolicyBuild(PolicyControlContext& context)
{
	for (auto builder : policyBuilder) {
		builder(context);
	}
}

#define DEFINE_POLICY(__policy__)  \
	PolicyBuilder<__policy__> __policy__##_builder

#endif //!__POLICY_BUILDER_H__
