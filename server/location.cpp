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

#include <location_batch.h>

#include "logger.h"
#include "privilege.h"
#include "policy-builder.h"
#include "policy-model.h"

#include "location.hxx"

namespace DevicePolicyManager {

class LocationEnforceModel : public BaseEnforceModel {
public:
	LocationEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool enable)
	{
		if (location_manager_enable_restriction(!enable) == LOCATIONS_ERROR_NONE) {
			notify(enable == 0 ? "disallowed" : "allowed");
			return true;
		}

		return false;
	}
};

typedef GlobalPolicy<int, LocationEnforceModel> LocationStatePolicy;

struct LocationPolicy::Private : public PolicyHelper {
	Private(PolicyControlContext& ctxt) : PolicyHelper(ctxt) {}
	LocationStatePolicy location{context, "location"};
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

LocationPolicy::LocationPolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_LOCATION, (int)(LocationPolicy::setLocationState)(bool));
	context.expose(this, "", (bool)(LocationPolicy::getLocationState)());
}

LocationPolicy::~LocationPolicy()
{
}

int LocationPolicy::setLocationState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->location, enable);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

bool LocationPolicy::getLocationState()
{
	return pimpl->getPolicy(pimpl->location);
}

DEFINE_POLICY(LocationPolicy);

} // namespace DevicePolicyManager
