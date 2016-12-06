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

#include "privilege.h"
#include "policy-builder.h"

#include "location.hxx"

namespace DevicePolicyManager {

LocationPolicy::LocationPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
	context.registerParametricMethod(this, DPM_PRIVILEGE_LOCATION, (int)(LocationPolicy::setLocationState)(bool));
	context.registerNonparametricMethod(this, "", (bool)(LocationPolicy::getLocationState));

	context.createNotification("location");
}

LocationPolicy::~LocationPolicy()
{
}

int LocationPolicy::setLocationState(bool enable)
{
	if (!SetPolicyAllowed(context, "location", enable)) {
		return 0;
	}

	if (location_manager_enable_restriction(!enable) != LOCATIONS_ERROR_NONE) {
		return -1;
	}

	return 0;
}

bool LocationPolicy::getLocationState()
{
	return context.getPolicy<int>("location");
}

DEFINE_POLICY(LocationPolicy);

} // namespace DevicePolicyManager
