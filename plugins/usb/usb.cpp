/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <policyd/pil/policy-context.h>
#include <policyd/pil/policy-model.h>
#include <policyd/pil/policy-storage.h>
#include <policyd/pil/policy-event.h>
#include <policyd/pil/app-bundle.h>
#include <policyd/pil/launchpad.h>

#include <klay/auth/user.h>
#include <klay/dbus/variant.h>
#include <klay/dbus/connection.h>

#include <tzplatform_config.h>

#include "../dlog.h"

#define DEVICED_SYSNOTI_INTERFACE		\
	"org.tizen.system.deviced",				\
	"/Org/Tizen/System/DeviceD/SysNoti",	\
	"org.tizen.system.deviced.SysNoti",			\
	"control"

class DebuggingMode : public GlobalPolicy<DataSetInt> {
public:
	DebuggingMode() : GlobalPolicy("usb-debugging")
	{
		PolicyEventNotifier::create("usb_debugging");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		PolicyEventNotifier::emit("usb_debugging", enable ? "allowed" : "disallowed");
		return true;
	}
};

class Tethering : public GlobalPolicy<DataSetInt> {
public:
	Tethering() : GlobalPolicy("usb-tethering")
	{
		PolicyEventNotifier::create("usb_tethering");
	}

	bool apply(const DataType& value)
	{
		int enable = value;
		PolicyEventNotifier::emit("usb_tethering", enable ? "allowed" : "disallowed");
		return true;
	}
};

class Client : public GlobalPolicy<DataSetInt> {
public:
	Client() : GlobalPolicy("usb-client")
	{
		PolicyEventNotifier::create("usb_client");
		sendDbusSignal();
	}

	bool apply(const DataType& value)
	{
		int ret;
		int enable = value;

		try {
			std::string pid(std::to_string(::getpid()));
			std::string state(std::to_string(enable));
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(DEVICED_SYSNOTI_INTERFACE,
										-1, "(i)", "(sisss)",
										"control", 3, pid.c_str(), "1", state.c_str()).get("(i)", &ret);
		} catch(runtime::Exception& e) {
			ERROR(PLUGINS, "Failed to enforce usb client");
			return false;
		}

		if (ret == 0) {
			PolicyEventNotifier::emit("usb_client", enable ? "allowed" : "disallowed");
			return true;
		}

		return false;
	}

	void sendDbusSignal(void)
	{
		int ret;
		int enable = get().value;

		try {
			std::string pid(std::to_string(::getpid()));
			std::string state(std::to_string(enable));
			dbus::Connection &systemDBus = dbus::Connection::getSystem();
			systemDBus.methodcall(DEVICED_SYSNOTI_INTERFACE,
										-1, "(i)", "(sisss)",
										"control", 3, pid.c_str(), "1", state.c_str()).get("(i)", &ret);
		} catch(runtime::Exception& e) {
			ERROR(PLUGINS, "Failed to enforce usb client");
		}
	}
};

class Usb : public AbstractPolicyProvider {
public:
	int setDebuggingState(bool enable);
	bool getDebuggingState();

	int setTetheringState(bool enable);
	bool getTetheringState();

	int setClientState(bool enable);
	bool getClientState();

private:
	DebuggingMode debugging;
	Tethering tethering;
	Client client;
};

int Usb::setDebuggingState(bool enable)
{
	try {
		debugging.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Usb::getDebuggingState()
{
	return debugging.get();
}

int Usb::setTetheringState(bool enable)
{
	try {
		tethering.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Usb::getTetheringState()
{
	return tethering.get();
}

int Usb::setClientState(bool enable)
{
	try {
		client.set(enable);
	} catch (runtime::Exception& e) {
		ERROR(PLUGINS, e.what());
		return -1;
	}

	return 0;
}

bool Usb::getClientState()
{
	return client.get();
}

extern "C" {

#define PRIVILEGE_USB       "http://tizen.org/privilege/dpm.usb"
#define PRIVILEGE_DEBUGGING "http://tizen.org/privilege/dpm.debugging"

AbstractPolicyProvider *PolicyFactory(PolicyControlContext& context)
{
	INFO(PLUGINS, "Usb plugin loaded");
	Usb *policy = new Usb();

	context.expose(policy, PRIVILEGE_DEBUGGING, (int)(Usb::setDebuggingState)(bool));
	context.expose(policy, PRIVILEGE_USB, (int)(Usb::setTetheringState)(bool));
	context.expose(policy, PRIVILEGE_USB, (int)(Usb::setClientState)(bool));

	context.expose(policy, "", (bool)(Usb::getDebuggingState)());
	context.expose(policy, "", (bool)(Usb::getTetheringState)());
	context.expose(policy, "", (bool)(Usb::getClientState)());

	return policy;
}

} // extern "C"
