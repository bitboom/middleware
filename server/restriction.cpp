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

#include <klay/exception.h>
#include <klay/dbus/connection.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"
#include "policy-model.h"

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

class RecordingEnforceModel : public BaseEnforceModel {
public:
	RecordingEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool value)
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
								  !value).get("(s)", &result);
		} catch (runtime::Exception& e) {
			ERROR("Failed to enforce microphone policy");
			return false;
		}

		if (strcmp(result, "STREAM_MANAGER_RETURN_OK") != 0) {
			return false;
		}

		notify(value == 0 ? "disallowed" : "allowed");
		return true;
	}
};

class StorageEnforceModel : public BaseEnforceModel {
public:
	StorageEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool value)
	{
		int ret;
		try {
			std::string pid(std::to_string(::getpid()));
			std::string state(std::to_string(value));
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(DEVICED_SYSNOTI_INTERFACE,
								  -1, "(i)", "(sisss)",
								  "control", 3, pid.c_str(), "2", state.c_str()).get("(i)", &ret);
		} catch(runtime::Exception& e) {
			ERROR("Failed to enforce external storage policy");
			return false;
		}

		if (ret == 0) {
			notify(value == 0 ? "disallowed" : "allowed");
			return true;
		}

		return false;
	}
};

class StateEnforceModel : public BaseEnforceModel {
public:
	StateEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, name)
	{
	}

	bool operator()(bool value, uid_t domain = 0)
	{
		notify(value == 0 ? "disallowed" : "allowed");
		return true;
	}
};

typedef GlobalPolicy<int, StorageEnforceModel> ExternalStoragePolicy;
typedef GlobalPolicy<int, RecordingEnforceModel> MicrophonePolicy;
typedef GlobalPolicy<int, StateEnforceModel> CameraPolicy;
typedef GlobalPolicy<int, StateEnforceModel> UsbDebuggingPolicy;
typedef GlobalPolicy<int, StateEnforceModel> UsbTetheringPolicy;
typedef GlobalPolicy<int, StateEnforceModel> EmailPolicy;
typedef GlobalPolicy<int, StateEnforceModel> MessagingPolicy;
typedef DomainPolicy<int, StateEnforceModel> ClipboardPolicy;
typedef DomainPolicy<int, StateEnforceModel> BrowserPolicy;

struct RestrictionPolicy::Private : public PolicyHelper {
	Private(PolicyControlContext& ctxt) : PolicyHelper(ctxt) {}

	CameraPolicy          camera{context, "camera"};
	ClipboardPolicy       clipboard{context, "clipboard"};
	ExternalStoragePolicy storage{context, "external-storage"};
	MicrophonePolicy      microphone{context, "microphone"};
	UsbDebuggingPolicy    usbDebugging{context, "usb-debugging"};
	UsbTetheringPolicy    usbTethering{context, "usb-tethering"};
	EmailPolicy           email{context, "popimap-email"};
	MessagingPolicy       messaging{context, "messaging"};
	BrowserPolicy         browser{context, "browser"};
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
}

RestrictionPolicy::~RestrictionPolicy()
{
}

int RestrictionPolicy::setCameraState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->camera, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getCameraState()
{
	return pimpl->getPolicy(pimpl->camera);
}

int RestrictionPolicy::setMicrophoneState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->microphone, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getMicrophoneState()
{
	return pimpl->getPolicy(pimpl->microphone);
}

int RestrictionPolicy::setClipboardState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->clipboard, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getClipboardState()
{
	return pimpl->getPolicy(pimpl->clipboard);
}

int RestrictionPolicy::setUsbDebuggingState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->usbDebugging, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getUsbDebuggingState()
{
	return pimpl->getPolicy(pimpl->usbDebugging);
}

int RestrictionPolicy::setUsbTetheringState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->usbTethering, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getUsbTetheringState()
{
	return pimpl->getPolicy(pimpl->usbTethering);
}

int RestrictionPolicy::setExternalStorageState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->storage, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getExternalStorageState()
{
	return pimpl->getPolicy(pimpl->storage);
}

int RestrictionPolicy::setPopImapEmailState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->email, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getPopImapEmailState()
{
	return pimpl->getPolicy(pimpl->email);
}

int RestrictionPolicy::setMessagingState(const std::string& sim_id, bool enable)
{
	try {
		pimpl->setPolicy(pimpl->messaging, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getMessagingState(const std::string& sim_id)
{
	return pimpl->getPolicy(pimpl->messaging);
}

int RestrictionPolicy::setBrowserState(bool enable)
{
	try {
		pimpl->setPolicy(pimpl->browser, enable);
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

bool RestrictionPolicy::getBrowserState()
{
	return pimpl->getPolicy(pimpl->browser);
}

DEFINE_POLICY(RestrictionPolicy);

} // namespace DevicePolicyManager
