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

#include "status.h"
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

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::setModeChangeState", enable);

	return status.get();
}

bool BluetoothPolicy::getModeChangeState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("BluetoothPolicy::getModeChangeState");

	return status.get();
}

int BluetoothPolicy::setDesktopConnectivityState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<bool>("BluetoothPolicy::setDesktopConnectivityState", enable);

	return status.get();
}

bool BluetoothPolicy::getDesktopConnectivityState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("BluetoothPolicy::getDesktopConnectivityState");

	return status.get();
}

int BluetoothPolicy::setTetheringState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::setTetheringState", enable);

	return status.get();
}

bool BluetoothPolicy::getTetheringState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("BluetoothPolicy::getTetheringState");

	return status.get();
}

int BluetoothPolicy::setPairingState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::setPairingState", enable);

	return status.get();
}

bool BluetoothPolicy::getPairingState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("BluetoothPolicy::getPairingState");

	return status.get();
}

int BluetoothPolicy::addDeviceToBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::addDeviceToBlacklist", mac);

	return status.get();
}

int BluetoothPolicy::removeDeviceFromBlacklist(const std::string& mac)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::removeDeviceFromBlacklist", mac);

	return status.get();
}

int BluetoothPolicy::setDeviceRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::setDeviceRestriction", enable);

	return status.get();
}

bool BluetoothPolicy::isDeviceRestricted()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { false };

	status = context.methodCall<bool>("BluetoothPolicy::isDeviceRestricted");

	return status.get();
}

int BluetoothPolicy::addUuidToBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::addUuidToBlacklist", uuid);

	return status.get();
}

int BluetoothPolicy::removeUuidFromBlacklist(const std::string& uuid)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::removeUuidFromBlacklist", uuid);

	return status.get();
}

int BluetoothPolicy::setUuidRestriction(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("BluetoothPolicy::setUuidRestriction", enable);

	return status.get();
}

bool BluetoothPolicy::isUuidRestricted()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { false };

	status = context.methodCall<bool>("BluetoothPolicy::isUuidRestricted");

	return status.get();
}

} // namespace DevicePolicyManager
