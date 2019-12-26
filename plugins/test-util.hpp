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

#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-value.hpp>

#include <memory>

namespace vist {
namespace test {

using namespace vist::policy;

template <typename T>
void change_policy_state()
{
	std::shared_ptr<PolicyModel> policy = std::make_shared<T>();
	/// enable policy
	policy->onChanged(PolicyValue(1));
	/// disable policy
	policy->onChanged(PolicyValue(0));
}

} // namespace test
} // namespace vist
