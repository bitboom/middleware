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

#include "policy-event.h"
#include "policy-event-env.h"
#include "logger.h"

#include <klay/dbus/signal.h>
#include <klay/dbus/introspection.h>
#include <klay/exception.h>


void PolicyEventNotifier::init(void) noexcept
{
	try {
		auto manifest = dbus::Introspection::createXmlDataFromFile(PIL_MANIFEST_PATH);
		dbus::Introspection is(manifest);
		is.addInterface(PIL_EVENT_INTERFACE);

		manifest = is.getXmlData();
		dbus::Introspection::writeXmlDataToFile(PIL_MANIFEST_PATH, manifest);

		dbus::Connection& conn = dbus::Connection::getSystem();
		conn.registerObject(PIL_OBJECT_PATH, manifest, nullptr, nullptr);
		DEBUG(DPM, "Success to init event-notifier.");
	} catch(runtime::Exception& e) {
		ERROR(DPM, e.what());
	}
}

void PolicyEventNotifier::create(const std::string& name) noexcept
{
	try {
		dbus::signal::Sender sender(PIL_OBJECT_PATH, PIL_EVENT_INTERFACE);
		sender.addSignal(PIL_MANIFEST_PATH, name, "(s)");
		DEBUG(DPM, "Success to create new event: " << name);
	} catch(runtime::Exception& e) {
		ERROR(DPM, e.what() << ", name: " << name);
	}
}

void PolicyEventNotifier::emit(const std::string& name, const std::string& state) noexcept
{
	try {
		dbus::signal::Sender sender(PIL_OBJECT_PATH, PIL_EVENT_INTERFACE);
		sender.emit(name, "(s)", state.c_str());
		DEBUG(DPM, "Event occured name: " << name << ", state: " << state);
	} catch(runtime::Exception& e) {
		ERROR(DPM, e.what() << ", name: " << name << ", state: " << state);
	}
}
