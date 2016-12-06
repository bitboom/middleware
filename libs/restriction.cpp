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

namespace DevicePolicyManager
{

RestrictionPolicy::RestrictionPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setMicrophoneState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setMicrophoneState", enable);
}

bool RestrictionPolicy::getMicrophoneState()
{
	return context->methodCall<bool>("RestrictionPolicy::getMicrophoneState");
}

int RestrictionPolicy::setCameraState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setCameraState", enable);
}

bool RestrictionPolicy::getCameraState()
{
	return context->methodCall<bool>("RestrictionPolicy::getCameraState");
}

int RestrictionPolicy::setClipboardState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setClipboardState", enable);
}

bool RestrictionPolicy::getClipboardState()
{
	return context->methodCall<bool>("RestrictionPolicy::getClipboardState");
}

int RestrictionPolicy::setUsbDebuggingState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setUsbDebuggingState", enable);
}

bool RestrictionPolicy::getUsbDebuggingState()
{
	return context->methodCall<bool>("RestrictionPolicy::getUsbDebuggingState");
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setUsbTetheringState", enable);
}
bool RestrictionPolicy::getUsbTetheringState()
{
	return context->methodCall<bool>("RestrictionPolicy::getUsbTetheringState");
}

int RestrictionPolicy::setExternalStorageState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setExternalStorageState", enable);
}

bool RestrictionPolicy::getExternalStorageState()
{
	return context->methodCall<bool>("RestrictionPolicy::getExternalStorageState");
}

int RestrictionPolicy::setPopImapEmailState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setPopImapEmailState", enable);
}

bool RestrictionPolicy::getPopImapEmailState()
{
	return context->methodCall<bool>("RestrictionPolicy::getPopImapEmailState");
}

int RestrictionPolicy::setMessagingState(const std::string &sim_id, bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setMessagingState", sim_id, enable);
}

bool RestrictionPolicy::getMessagingState(const std::string &sim_id)
{
	return context->methodCall<bool>("RestrictionPolicy::getMessagingState", sim_id);
}

int RestrictionPolicy::setBrowserState(bool enable)
{
	return context->methodCall<int>("RestrictionPolicy::setBrowserState", enable);
}

bool RestrictionPolicy::getBrowserState()
{
	return context->methodCall<bool>("RestrictionPolicy::getBrowserState");
}

} //namespace DevicePolicyManager
