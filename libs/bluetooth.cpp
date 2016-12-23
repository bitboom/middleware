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
#include "bluetooth.hxx"

namespace DevicePolicyManager {

struct BluetoothPolicy::Private {
	Private(PolicyControlContext& ctx) : context(ctx) {}
	PolicyControlContext& context;
};

BluetoothPolicy::BluetoothPolicy(BluetoothPolicy&& rhs) = default;
BluetoothPolicy& BluetoothPolicy::operator=(BluetoothPolicy&& rhs) = default;

BluetoothPolicy::BluetoothPolicy(const BluetoothPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

BluetoothPolicy& BluetoothPolicy::operator=(const BluetoothPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

BluetoothPolicy::BluetoothPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

BluetoothPolicy::~BluetoothPolicy()
{
}

// for restriction CPIs
int BluetoothPolicy::setModeChangeState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::setModeChangeState", enable);
}

bool BluetoothPolicy::getModeChangeState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::getModeChangeState");
}

int BluetoothPolicy::setDesktopConnectivityState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::setDesktopConnectivityState", enable);
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::getDesktopConnectivityState");
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::setTetheringState", enable);
}

bool BluetoothPolicy::getTetheringState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::getTetheringState");
}

int BluetoothPolicy::setPairingState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::setPairingState", enable);
}

bool BluetoothPolicy::getPairingState()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::getPairingState");
}

int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::addDeviceToBlacklist", mac);
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::removeDeviceFromBlacklist", mac);
}

int BluetoothPolicy::setDeviceRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::setDeviceRestriction", enable);
}

bool BluetoothPolicy::isDeviceRestricted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::isDeviceRestricted");
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::addUuidToBlacklist", uuid);
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::removeUuidFromBlacklist", uuid);
}

int BluetoothPolicy::setUuidRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("BluetoothPolicy::setUuidRestriction", enable);
}

bool BluetoothPolicy::isUuidRestricted()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<bool>("BluetoothPolicy::isUuidRestricted");
}

} // namespace DevicePolicyManager
