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
	return context->methodCall<int>("WifiPolicy::setState", enable);
}

bool WifiPolicy::getState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("WifiPolicy::getState");
}

int WifiPolicy::setHotspotState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("WifiPolicy::setHotspotState", enable);
}

bool WifiPolicy::getHotspotState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("WifiPolicy::getHotspotState");
}

int WifiPolicy::setProfileChangeRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("WifiPolicy::setProfileChangeRestriction", enable);
}

bool WifiPolicy::isProfileChangeRestricted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("WifiPolicy::isProfileChangeRestricted");
}

int WifiPolicy::setNetworkAccessRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("WifiPolicy::setNetworkAccessRestriction", enable);
}

bool WifiPolicy::isNetworkAccessRestricted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("WifiPolicy::isNetworkAccessRestricted");
}

int WifiPolicy::addSsidToBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("WifiPolicy::addSsidToBlocklist", ssid);
}

int WifiPolicy::removeSsidFromBlocklist(const std::string& ssid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("WifiPolicy::removeSsidFromBlocklist", ssid);
}

} // namespace DevicePolicyManager
