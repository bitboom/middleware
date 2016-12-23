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

#include "administration.hxx"

namespace DevicePolicyManager {

struct AdministrationPolicy::Private {
	Private(PolicyControlContext& ctx) : context(ctx) {}
	PolicyControlContext& context;
};

AdministrationPolicy::AdministrationPolicy(AdministrationPolicy&& rhs) = default;
AdministrationPolicy& AdministrationPolicy::operator=(AdministrationPolicy&& rhs) = default;

AdministrationPolicy::AdministrationPolicy(const AdministrationPolicy& rhs) :
	pimpl(nullptr)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

AdministrationPolicy& AdministrationPolicy::operator=(const AdministrationPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

AdministrationPolicy::AdministrationPolicy(PolicyControlContext& ctx) :
	pimpl(new Private(ctx))
{
}

AdministrationPolicy::~AdministrationPolicy()
{
}

int AdministrationPolicy::registerPolicyClient(const std::string& name, uid_t uid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("AdministrationPolicy::registerPolicyClient", name, uid);
}

int AdministrationPolicy::deregisterPolicyClient(const std::string& name, uid_t uid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("AdministrationPolicy::deregisterPolicyClient", name, uid);
}

} // namespace DevicePolicyManager
