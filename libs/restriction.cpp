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
#include "restriction.hxx"

namespace DevicePolicyManager {

struct RestrictionPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

RestrictionPolicy::RestrictionPolicy(RestrictionPolicy&& rhs) = default;
RestrictionPolicy& RestrictionPolicy::operator=(RestrictionPolicy&& rhs) = default;

RestrictionPolicy::RestrictionPolicy(const RestrictionPolicy& rhs)
{
	std::cout << "Copy Constructor" << std::endl;
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

RestrictionPolicy& RestrictionPolicy::operator=(const RestrictionPolicy& rhs)
{
	std::cout << "Copy Operator" << std::endl;
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

RestrictionPolicy::RestrictionPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setMicrophoneState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setMicrophoneState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getMicrophoneState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getMicrophoneState");
	}

	return true;
}

int RestrictionPolicy::setCameraState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setCameraState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getCameraState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getCameraState");
	}

	return true;
}

int RestrictionPolicy::setClipboardState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setClipboardState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getClipboardState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getClipboardState");
	}

	return true;
}

int RestrictionPolicy::setUsbDebuggingState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setUsbDebuggingState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getUsbDebuggingState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getUsbDebuggingState");
	}

	return true;
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setUsbTetheringState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getUsbTetheringState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getUsbTetheringState");
	}

	return true;
}

int RestrictionPolicy::setExternalStorageState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setExternalStorageState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getExternalStorageState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getExternalStorageState");
	}

	return true;
}

int RestrictionPolicy::setPopImapEmailState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setPopImapEmailState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getPopImapEmailState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getPopImapEmailState");
	}

	return true;
}

int RestrictionPolicy::setMessagingState(const std::string &sim_id, bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setMessagingState", sim_id, enable);
	}

	return -1;
}

bool RestrictionPolicy::getMessagingState(const std::string &sim_id)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getMessagingState", sim_id);
	}

	return true;
}

int RestrictionPolicy::setBrowserState(bool enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("RestrictionPolicy::setBrowserState", enable);
	}

	return -1;
}

bool RestrictionPolicy::getBrowserState()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<bool>("RestrictionPolicy::getBrowserState");
	}

	return true;
}

} //namespace DevicePolicyManager
