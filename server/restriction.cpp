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

#include <vconf.h>
#include <klay/exception.h>
#include <klay/dbus/connection.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"

#include "restriction.hxx"

#define PULSEAUDIO_LOGIN_INTERFACE          \
	"org.pulseaudio.Server",                \
	"/org/pulseaudio/StreamManager",        \
	"org.pulseaudio.StreamManager"

#define DEVICED_SYSNOTI_INTERFACE           \
	"org.tizen.system.deviced",             \
	"/Org/Tizen/System/DeviceD/SysNoti",    \
	"org.tizen.system.deviced.SysNoti",     \
	"control"

namespace DevicePolicyManager {

namespace {

std::vector<std::string> restrictionNotifications = {
	"camera",
	"clipboard",
	"external-storage",
	"microphone",
	"usb-debugging",
	"usb-tethering",
	"popimap-email",
	"messaging",
	"browser"
};

} // namespace

struct RestrictionPolicy::Private {
	Private(PolicyControlContext& ctxt) :
		context(ctxt)
	{
	}

	int setState(const std::string& name, int allow, bool* updated = nullptr)
	{
		try {
			bool ret = SetPolicyAllowed(context, name, allow);
			if (updated) {
				*updated = ret;
			}
		} catch (runtime::Exception& e) {
			ERROR("Failed to set " << name << "policy");
			return -1;
		}

		return 0;
	}

	bool getState(const std::string& name)
	{
		return context.getPolicy<int>(name);
	}

	PolicyControlContext& context;
};

RestrictionPolicy::RestrictionPolicy(RestrictionPolicy&& rhs) = default;
RestrictionPolicy& RestrictionPolicy::operator=(RestrictionPolicy&& rhs) = default;

RestrictionPolicy::RestrictionPolicy(const RestrictionPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

RestrictionPolicy& RestrictionPolicy::operator=(const RestrictionPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

RestrictionPolicy::RestrictionPolicy(PolicyControlContext& context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_CAMERA, (int)(RestrictionPolicy::setCameraState)(bool));
	context.expose(this, DPM_PRIVILEGE_MICROPHONE, (int)(RestrictionPolicy::setMicrophoneState)(bool));
	context.expose(this, DPM_PRIVILEGE_CLIPBOARD, (int)(RestrictionPolicy::setClipboardState)(bool));
	context.expose(this, DPM_PRIVILEGE_DEBUGGING, (int)(RestrictionPolicy::setUsbDebuggingState)(bool));
	context.expose(this, DPM_PRIVILEGE_USB, (int)(RestrictionPolicy::setUsbTetheringState)(bool));
	context.expose(this, DPM_PRIVILEGE_STORAGE, (int)(RestrictionPolicy::setExternalStorageState)(bool));
	context.expose(this, DPM_PRIVILEGE_EMAIL, (int)(RestrictionPolicy::setPopImapEmailState)(bool));
	context.expose(this, DPM_PRIVILEGE_MESSAGING, (int)(RestrictionPolicy::setMessagingState)(std::string, bool));
	context.expose(this, DPM_PRIVILEGE_BROWSER, (int)(RestrictionPolicy::setBrowserState)(bool));

	context.expose(this, "", (bool)(RestrictionPolicy::getCameraState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getMicrophoneState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getClipboardState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getUsbDebuggingState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getUsbTetheringState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getExternalStorageState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getPopImapEmailState)());
	context.expose(this, "", (bool)(RestrictionPolicy::getBrowserState)());
	context.expose(this, "", (int)(RestrictionPolicy::getMessagingState)(std::string));

	context.createNotification(restrictionNotifications);
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setCameraState(bool enable)
{
	return pimpl->setState("camera", enable);
}

bool RestrictionPolicy::getCameraState()
{
	return pimpl->getState("camera");
}

int RestrictionPolicy::setMicrophoneState(bool enable)
{
	bool updated = false;
	if (pimpl->setState("microphone", enable, &updated) != 0) {
		return -1;
	}

	if (!updated) {
		return 0;
	}

	char *result = NULL;
	try {
		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(PULSEAUDIO_LOGIN_INTERFACE,
							  "UpdateRestriction",
							  -1,
							  "(s)",
							  "(su)",
							  "block_recording_media",
							  !enable).get("(s)", &result);
	} catch (runtime::Exception& e) {
		ERROR("Failed to enforce microphone policy");
		return -1;
	}

	if (strcmp(result, "STREAM_MANAGER_RETURN_OK") != 0) {
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getMicrophoneState()
{
	return pimpl->getState("microphone");
}

int RestrictionPolicy::setClipboardState(bool enable)
{
	return pimpl->setState("clipboard", enable);
}

bool RestrictionPolicy::getClipboardState()
{
	return pimpl->getState("clipboard");
}

int RestrictionPolicy::setUsbDebuggingState(bool enable)
{
	return pimpl->setState("usb-debugging", enable);
}

bool RestrictionPolicy::getUsbDebuggingState()
{
	return pimpl->getState("usb-debugging");
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	return pimpl->setState("usb-tethering", enable);
}

bool RestrictionPolicy::getUsbTetheringState()
{
	return pimpl->getState("usb-tethering");
}

int RestrictionPolicy::setExternalStorageState(bool enable)
{
	bool updated = false;
	if (pimpl->setState("external-storage", enable, &updated) != 0) {
		return -1;
	}

	if (!updated) {
		return 0;
	}

	int ret;
	try {
		std::string pid(std::to_string(::getpid()));
		std::string state(std::to_string(enable));
		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(DEVICED_SYSNOTI_INTERFACE,
							  -1, "(i)", "(sisss)",
							  "control", 3, pid.c_str(), "2", state.c_str()).get("(i)", &ret);
	} catch(runtime::Exception& e) {
		ERROR("Failed to enforce external storage policy");
		return -1;
	}

	if (ret != 0) {
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getExternalStorageState()
{
	return pimpl->getState("external-storage");
}

int RestrictionPolicy::setPopImapEmailState(bool enable)
{
	return pimpl->setState("popimap-email", enable);
}

bool RestrictionPolicy::getPopImapEmailState()
{
	return pimpl->getState("popimap-email");
}

int RestrictionPolicy::setMessagingState(const std::string& sim_id, bool enable)
{
	return pimpl->setState("messaging", enable);
}

bool RestrictionPolicy::getMessagingState(const std::string& sim_id)
{
	return pimpl->getState("messaging");
}

int RestrictionPolicy::setBrowserState(bool enable)
{
	return pimpl->setState("browser", enable);
}

bool RestrictionPolicy::getBrowserState()
{
	return pimpl->getState("browser");
}

DEFINE_POLICY(RestrictionPolicy);

} // namespace DevicePolicyManager
