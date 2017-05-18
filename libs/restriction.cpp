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

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setMicrophoneState", enable);

	return status.get();
}

bool RestrictionPolicy::getMicrophoneState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getMicrophoneState");

	return status.get();
}

int RestrictionPolicy::setCameraState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setCameraState", enable);

	return status.get();
}

bool RestrictionPolicy::getCameraState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getCameraState");

	return status.get();
}

int RestrictionPolicy::setClipboardState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setClipboardState", enable);

	return status.get();
}

bool RestrictionPolicy::getClipboardState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getClipboardState");

	return status.get();
}

int RestrictionPolicy::setUsbDebuggingState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setUsbDebuggingState", enable);

	return status.get();
}

bool RestrictionPolicy::getUsbDebuggingState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getUsbDebuggingState");

	return status.get();
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setUsbTetheringState", enable);

	return status.get();
}

bool RestrictionPolicy::getUsbTetheringState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getUsbTetheringState");

	return status.get();
}

int RestrictionPolicy::setExternalStorageState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setExternalStorageState", enable);

	return status.get();
}

bool RestrictionPolicy::getExternalStorageState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getExternalStorageState");

	return status.get();
}

int RestrictionPolicy::setPopImapEmailState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setPopImapEmailState", enable);

	return status.get();
}

bool RestrictionPolicy::getPopImapEmailState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getPopImapEmailState");

	return status.get();
}

int RestrictionPolicy::setMessagingState(const std::string &sim_id, bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setMessagingState", sim_id, enable);

	return status.get();
}

bool RestrictionPolicy::getMessagingState(const std::string &sim_id)
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getMessagingState", sim_id);

	return status.get();
}

int RestrictionPolicy::setBrowserState(bool enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("RestrictionPolicy::setBrowserState", enable);

	return status.get();
}

bool RestrictionPolicy::getBrowserState()
{
	PolicyControlContext& context = pimpl->context;

	Status<bool> status { true };

	status = context.methodCall<bool>("RestrictionPolicy::getBrowserState");

	return status.get();
}

} //namespace DevicePolicyManager
