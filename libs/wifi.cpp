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

#include "status.h"
#include "wifi.hxx"

namespace DevicePolicyManager {

struct WifiPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

WifiPolicy::WifiPolicy(WifiPolicy&& rhs) = default;
WifiPolicy& WifiPolicy::operator=(WifiPolicy&& rhs) = default;

WifiPolicy::WifiPolicy(const WifiPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}


WifiPolicy& WifiPolicy::operator=(const WifiPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

WifiPolicy::WifiPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

WifiPolicy::~WifiPolicy()
{
}

int WifiPolicy::setState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::setState", enable);

	return status.get();
}

bool WifiPolicy::getState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("WifiPolicy::getState");

	return status.get();
}

int WifiPolicy::setHotspotState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::setHotspotState", enable);

	return status.get();
}

bool WifiPolicy::getHotspotState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("WifiPolicy::getHotspotState");

	return status.get();
}

int WifiPolicy::setProfileChangeRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::setProfileChangeRestriction", enable);

	return status.get();
}

bool WifiPolicy::isProfileChangeRestricted()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { false };

	status = context.methodCall<bool>("WifiPolicy::isProfileChangeRestricted");

	return status.get();
}

int WifiPolicy::setNetworkAccessRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::setNetworkAccessRestriction", enable);

	return status.get();
}

bool WifiPolicy::isNetworkAccessRestricted()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { false };

	status = context.methodCall<bool>("WifiPolicy::isNetworkAccessRestricted");

	return status.get();
}

int WifiPolicy::addSsidToBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::addSsidToBlocklist", ssid);

	return status.get();
}

int WifiPolicy::removeSsidFromBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("WifiPolicy::removeSsidFromBlocklist", ssid);

	return status.get();
}

} // namespace DevicePolicyManager
