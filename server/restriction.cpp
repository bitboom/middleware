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

RestrictionPolicy::RestrictionPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
	context.registerParametricMethod(this, DPM_PRIVILEGE_CAMERA, (int)(RestrictionPolicy::setCameraState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_MICROPHONE, (int)(RestrictionPolicy::setMicrophoneState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_CLIPBOARD, (int)(RestrictionPolicy::setClipboardState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_DEBUGGING, (int)(RestrictionPolicy::setUsbDebuggingState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_USB, (int)(RestrictionPolicy::setUsbTetheringState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_STORAGE, (int)(RestrictionPolicy::setExternalStorageState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_EMAIL, (int)(RestrictionPolicy::setPopImapEmailState)(int));
	context.registerParametricMethod(this, DPM_PRIVILEGE_MESSAGING, (int)(RestrictionPolicy::setMessagingState)(std::string, int));
	context.registerParametricMethod(this, "", (int)(RestrictionPolicy::getMessagingState)(std::string));
	context.registerParametricMethod(this, DPM_PRIVILEGE_BROWSER, (int)(RestrictionPolicy::setBrowserState)(int));

	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getCameraState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getMicrophoneState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getClipboardState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getUsbDebuggingState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getUsbTetheringState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getExternalStorageState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getPopImapEmailState));
	context.registerNonparametricMethod(this, "", (int)(RestrictionPolicy::getBrowserState));

	context.createNotification(restrictionNotifications);
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setCameraState(int enable)
{
	try {
		SetPolicyAllowed(context, "camera", enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to enforce camera policy");
		return -1;
	}
	return 0;
}

int RestrictionPolicy::getCameraState()
{
	return context.getPolicy<int>("camera");
}

int RestrictionPolicy::setMicrophoneState(int enable)
{
	char *result = NULL;
	try {
		if (!SetPolicyAllowed(context, "microphone", enable)) {
			return 0;
		}

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

int RestrictionPolicy::getMicrophoneState()
{
	return context.getPolicy<int>("microphone");
}

int RestrictionPolicy::setClipboardState(int enable)
{
	try {
		SetPolicyAllowed(context, "clipboard", enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to enforce clipboard policy");
		return -1;
	}
	return 0;
}

int RestrictionPolicy::getClipboardState()
{
	return context.getPolicy<int>("clipboard", context.getPeerUid());
}

int RestrictionPolicy::setUsbDebuggingState(int enable)
{
	try {
		SetPolicyAllowed(context, "usb-debugging", enable);
	} catch (runtime::Exception& e) {
		ERROR("Failed to enforce usb debugging policy");
		return -1;
	}
	return 0;
}

int RestrictionPolicy::getUsbDebuggingState()
{
	return context.getPolicy<int>("usb-debugging");
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	try {
		if (!SetPolicyAllowed(context, "usb-tethering", enable)) {
			return 0;
		}

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
	return 0;
}

bool RestrictionPolicy::getUsbTetheringState()
{
	return context.getPolicy<int>("usb-tethering");
}

int RestrictionPolicy::setExternalStorageState(int enable)
{
	int ret;
	try {
		if (!SetPolicyAllowed(context, "external-storage", enable)) {
			return 0;
		}

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

int RestrictionPolicy::getExternalStorageState()
{
	return context.getPolicy<int>("external-storage");
}

int RestrictionPolicy::setPopImapEmailState(int enable)
{
	try {
		SetPolicyAllowed(context, "popimap-email", enable);
    } catch (runtime::Exception& e) {
        ERROR("Failed to enforce pop/imap email policy");
        return -1;
    }
    return 0;
}

int RestrictionPolicy::getPopImapEmailState()
{
	return context.getPolicy<int>("popimap-email");
}

int RestrictionPolicy::setMessagingState(const std::string& sim_id, int enable)
{
	try {
		SetPolicyAllowed(context, "messaging", enable);
    } catch (runtime::Exception& e) {
        ERROR("Failed to enforce messaging policy");
        return -1;
    }
	return 0;
}

int RestrictionPolicy::getMessagingState(const std::string& sim_id)
{
	return context.getPolicy<int>("messaging");
}

int RestrictionPolicy::setBrowserState(int enable)
{
	try {
		SetPolicyAllowed(context, "browser", enable);
	} catch (runtime::Exception& e) {
        ERROR("Failed to enforce browser policy");
        return -1;
    }
	return 0;
}

int RestrictionPolicy::getBrowserState()
{
	return context.getPolicy<int>("browser", context.getPeerUid());
}

DEFINE_POLICY(RestrictionPolicy);

} // namespace DevicePolicyManager
