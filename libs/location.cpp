/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "location.hxx"

namespace DevicePolicyManager {

struct LocationPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

LocationPolicy::LocationPolicy(LocationPolicy&& rhs) = default;
LocationPolicy& LocationPolicy::operator=(LocationPolicy&& rhs) = default;

LocationPolicy::LocationPolicy(const LocationPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

LocationPolicy& LocationPolicy::operator=(const LocationPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

LocationPolicy::LocationPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

LocationPolicy::~LocationPolicy()
{
}

int LocationPolicy::setLocationState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("LocationPolicy::setLocationState", enable);
	}

	return -1;
}

bool LocationPolicy::getLocationState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("LocationPolicy::getLocationState");
	}

	return true;
}

} //namespace DevicePolicyManager
