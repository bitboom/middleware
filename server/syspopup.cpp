/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <stdexcept>

#include <tzplatform_config.h>

#include <klay/exception.h>
#include <klay/auth/user.h>
#include <klay/dbus/connection.h>
#include <klay/audit/logger.h>

#include "syspopup.h"
#include "bundle.h"
#include "launchpad.h"

using namespace std::placeholders;

namespace {

const std::string SYSPOPUP_BUS_NAME       = "org.tizen.DevicePolicyManager";
const std::string SYSPOPUP_OBJECT_PATH    = "/org/tizen/DevicePolicyManager/Syspopup";
const std::string SYSPOPUP_INTERFACE      = "org.tizen.DevicePolicyManager.Syspopup";
const std::string SYSPOPUP_METHOD_SHOW    = "show";

const std::string manifest =
	"<node>"
	"   <interface name='" + SYSPOPUP_INTERFACE + "'>"
	"       <method name='" + SYSPOPUP_METHOD_SHOW + "'>"
	"           <arg type='s' name='id' direction='in'/>"
	"           <arg type='b' name='result' direction='out'/>"
	"       </method>"
	"   </interface>"
	"</node>";

} // namespace

dbus::Variant SyspopupService::onMethodCall(const std::string& objectPath,
											const std::string& interface,
											const std::string& methodName,
											dbus::Variant parameters)
{
	if (objectPath != SYSPOPUP_OBJECT_PATH || interface != SYSPOPUP_INTERFACE) {
		throw runtime::Exception("Invalid Interface");
	}
	if (methodName != SYSPOPUP_METHOD_SHOW) {
		throw runtime::Exception("Unknown Method");
	}

	const gchar *id = NULL;
	parameters.get("(&s)", &id);

	Bundle bundle;
	bundle.add("id", id);

	Launchpad launchpad(activeUser);
	launchpad.launch("org.tizen.dpm-syspopup", bundle);

	return dbus::Variant("(b)", true);
}

void SyspopupService::onNameAcquired()
{
}

void SyspopupService::onNameLost()
{
}

SyspopupService::~SyspopupService()
{
}

SyspopupService::SyspopupService()
{
	runtime::User defaultUser(::tzplatform_getenv(TZ_SYS_DEFAULT_USER));
	activeUser = defaultUser.getUid();
}

void SyspopupService::run()
{
	dbus::Connection& connection = dbus::Connection::getSystem();
	connection.setName(SYSPOPUP_BUS_NAME,
					   std::bind(&SyspopupService::onNameAcquired, this),
					   std::bind(&SyspopupService::onNameLost, this));

	connection.registerObject(SYSPOPUP_OBJECT_PATH, manifest,
							  std::bind(&SyspopupService::onMethodCall, this, _1, _2, _3, _4),
							  nullptr);
}
