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

#define MOBILEAP_INTERFACE                  \
	"org.tizen.MobileapAgent",              \
	"/MobileapAgent",                       \
	"org.tizen.tethering"


namespace DevicePolicyManager {

RestrictionPolicy::RestrictionPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
	context.registerParametricMethod(this, DPM_PRIVILEGE_CAMERA, (int)(RestrictionPolicy::setCameraState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getCameraState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_MICROPHONE, (int)(RestrictionPolicy::setMicrophoneState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getMicrophoneState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_CLIPBOARD, (int)(RestrictionPolicy::setClipboardState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getClipboardState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_DEBUGGING, (int)(RestrictionPolicy::setUsbDebuggingState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getUsbDebuggingState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_USB, (int)(RestrictionPolicy::setUsbTetheringState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getUsbTetheringState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_STORAGE, (int)(RestrictionPolicy::setExternalStorageState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getExternalStorageState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_EMAIL, (int)(RestrictionPolicy::setPopImapEmailState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getPopImapEmailState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_MESSAGING, (int)(RestrictionPolicy::setMessagingState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getMessagingState));
	context.registerParametricMethod(this, DPM_PRIVILEGE_BROWSER, (int)(RestrictionPolicy::setBrowserState)(int));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getBrowserState));

	DefineAllowablePolicy(context, "camera");
	DefineAllowablePolicy(context, "clipboard");
	DefineAllowablePolicy(context, "external-storage");
	DefineAllowablePolicy(context, "microphone");
	DefineAllowablePolicy(context, "settings-changes");
	DefineAllowablePolicy(context, "usb-debugging");
	DefineAllowablePolicy(context, "usb-tethering");
	DefineAllowablePolicy(context, "popimap-email");
	DefineAllowablePolicy(context, "messaging");
	DefineAllowablePolicy(context, "browser");
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setCameraState(int enable)
{
	return SetPolicyAllowed(context, "camera", enable);
}

int RestrictionPolicy::getCameraState()
{
	return IsPolicyAllowed(context, "camera");
}

int RestrictionPolicy::setMicrophoneState(int enable)
{
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
		ERROR("Failed to enforce location policy");
		return -1;
	}

	if (strcmp(result, "STREAM_MANAGER_RETURN_OK") == 0) {
		return SetPolicyAllowed(context, "microphone", enable);
	}

	return -1;
}

int RestrictionPolicy::getMicrophoneState()
{
	return IsPolicyAllowed(context, "microphone");
}

int RestrictionPolicy::setClipboardState(int enable)
{
	return SetPolicyAllowed(context, "clipboard", enable);
}

int RestrictionPolicy::getClipboardState()
{
	return IsPolicyAllowed(context, "clipboard");
}

int RestrictionPolicy::setUsbDebuggingState(int enable)
{
	return SetPolicyAllowed(context, "usb-debugging", enable);
}

int RestrictionPolicy::getUsbDebuggingState()
{
	return IsPolicyAllowed(context, "usb-debugging");
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	try {
		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall(MOBILEAP_INTERFACE,
							  "change_policy",
							  -1,
							  "",
							  "(sb)",
							  "usb-tethering",
							  enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to change USB tethering state");
		return -1;
	}

	return SetPolicyAllowed(context, "usb-tethering", enable);
}

bool RestrictionPolicy::getUsbTetheringState()
{
	return IsPolicyAllowed(context, "usb-tethering");
}

int RestrictionPolicy::setExternalStorageState(int enable)
{
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

	if (ret == 0) {
		return SetPolicyAllowed(context, "external-storage", enable);
	}

	return -1;
}

int RestrictionPolicy::getExternalStorageState()
{
	return IsPolicyAllowed(context, "external-storage");
}

int RestrictionPolicy::setPopImapEmailState(int enable)
{
	return SetPolicyAllowed(context, "popimap-email", enable);
}

int RestrictionPolicy::getPopImapEmailState()
{
	return IsPolicyAllowed(context, "popimap-email");
}

int RestrictionPolicy::setMessagingState(int enable)
{
	return SetPolicyAllowed(context, "messaging", enable);
}

int RestrictionPolicy::getMessagingState()
{
	return IsPolicyAllowed(context, "messaging");
}

int RestrictionPolicy::setBrowserState(int enable)
{
	return SetPolicyAllowed(context, "browser", enable);
}

int RestrictionPolicy::getBrowserState()
{
	return IsPolicyAllowed(context, "browser");
}

DEFINE_POLICY(RestrictionPolicy);

} // namespace DevicePolicyManager
