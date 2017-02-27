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
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::setModeChangeState", enable);
	}

	return -1;
}

bool BluetoothPolicy::getModeChangeState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::getModeChangeState");
	}

	return true;
}

int BluetoothPolicy::setDesktopConnectivityState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::setDesktopConnectivityState", enable);
	}

	return -1;
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::getDesktopConnectivityState");
	}

	return true;
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::setTetheringState", enable);
	}

	return -1;
}

bool BluetoothPolicy::getTetheringState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::getTetheringState");
	}

	return true;
}

int BluetoothPolicy::setPairingState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::setPairingState", enable);
	}

	return -1;
}

bool BluetoothPolicy::getPairingState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::getPairingState");
	}

	return true;
}

int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::addDeviceToBlacklist", mac);
	}

	return -1;
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::removeDeviceFromBlacklist", mac);
	}

	return -1;
}

int BluetoothPolicy::setDeviceRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::setDeviceRestriction", enable);
	}

	return -1;
}

bool BluetoothPolicy::isDeviceRestricted()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::isDeviceRestricted");
	}

	return false;
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::addUuidToBlacklist", uuid);
	}

	return -1;
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::removeUuidFromBlacklist", uuid);
	}

	return -1;
}

int BluetoothPolicy::setUuidRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("BluetoothPolicy::setUuidRestriction", enable);
	}

	return -1;
}

bool BluetoothPolicy::isUuidRestricted()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("BluetoothPolicy::isUuidRestricted");
	}

	return false;
}

} // namespace DevicePolicyManager
