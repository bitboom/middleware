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
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::setState", enable);
	}


	return -1;
}

bool WifiPolicy::getState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("WifiPolicy::getState");
	}

	return true;
}

int WifiPolicy::setHotspotState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::setHotspotState", enable);
	}

	return -1;
}

bool WifiPolicy::getHotspotState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("WifiPolicy::getHotspotState");
	}

	return true;
}

int WifiPolicy::setProfileChangeRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::setProfileChangeRestriction", enable);
	}

	return -1;
}

bool WifiPolicy::isProfileChangeRestricted()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("WifiPolicy::isProfileChangeRestricted");
	}

	return false;
}

int WifiPolicy::setNetworkAccessRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::setNetworkAccessRestriction", enable);
	}

	return -1;
}

bool WifiPolicy::isNetworkAccessRestricted()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("WifiPolicy::isNetworkAccessRestricted");
	}

	return false;
}

int WifiPolicy::addSsidToBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::addSsidToBlocklist", ssid);
	}

	return -1;
}

int WifiPolicy::removeSsidFromBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("WifiPolicy::removeSsidFromBlocklist", ssid);
	}

	return -1;
}

} // namespace DevicePolicyManager
