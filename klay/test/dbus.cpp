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
#include <thread>
#include <memory>
#include <glib.h>
#include <iostream>

#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/dbus/variant.h>
#include <klay/dbus/signal.h>
#include <klay/dbus/connection.h>
#include <klay/dbus/introspection.h>
#include <klay/latch.h>

#include <klay/testbench.h>
#include <klay/gmainloop.h>

const std::string TESTSVC_BUS_NAME       = "org.tizen.klay";
const std::string TESTSVC_OBJECT_PATH    = "/org/tizen/klay";
const std::string TESTSVC_INTERFACE      = "test.api";
const std::string TESTSVC_METHOD_NOOP    = "Noop";
const std::string TESTSVC_METHOD_PROCESS = "Process";
const std::string TESTSVC_METHOD_THROW   = "Throw";
const std::string TESTSVC_SIGNAL_NOTIFY  = "Notify";
const std::string TESTSVC_NOT_EXIST      = "None";

const std::string TESTSVC_RUNTIME_OBJECT_PATH = "/org/tizen/klay/runtime";
const std::string TESTSVC_INTERFACE_NEW_NAME  = "interface.api";
const std::string TESTSVC_METHOD_NEW_NAME     = "NewMethod";
const std::string TESTSVC_SIGNAL_NEW_NAME     = "NewSignal";
const std::string TESTSVC_PROPERTY_NEW_NAME   = "NewProperty";

const std::string TESTSVC_METHOD_NEW_DATA =
	"<method name='" + TESTSVC_METHOD_NEW_NAME + "'>"
	"  <arg type='s' name='argument' direction='in'/>"
	"  <arg type='s' name='response' direction='out'/>"
	"</method>";
const std::string TESTSVC_SIGNAL_NEW_DATA =
	"<signal name='" + TESTSVC_SIGNAL_NEW_NAME + "'>"
	"  <arg type='s' name='argument'/>"
	"</signal>";
const std::string TESTSVC_PROPERTY_NEW_DATA =
	"<property name='" + TESTSVC_PROPERTY_NEW_NAME + "' type='y' access='readwrite'/>";

const std::string TESTSVC_WRONG_DATA_TYPE1 = "signal'/>";
const std::string TESTSVC_WRONG_DATA_TYPE2 = "<signal>";

const std::string TESTSVC_MANIFEST_PATH = TEST_DATA_DIR "/manifest";

const std::string manifest =
	"<node>"
	"	<interface name='" + TESTSVC_INTERFACE + "'>"
	"		<method name='" + TESTSVC_METHOD_NOOP + "'/>"
	"		<method name='" + TESTSVC_METHOD_PROCESS + "'>"
	"			<arg type='s' name='argument' direction='in'/>"
	"			<arg type='s' name='response' direction='out'/>"
	"		</method>"
	"		<method name='" + TESTSVC_METHOD_THROW + "'>"
	"			<arg type='i' name='argument' direction='in'/>"
	"		</method>"
	"		<signal name='" + TESTSVC_SIGNAL_NOTIFY + "'>"
	"			<arg type='s' name='argument'/>"
	"		</signal>"
	"	</interface>"
	"</node>";

TESTCASE(DbusNegativeTest)
{
	try {
		dbus::Connection &systemDBus = dbus::Connection::getSystem();
		systemDBus.methodcall("Unknown", "Unknown", "Unknown", "Unknown", -1, "", "");
	} catch (std::exception& e) {
	}
}

void signalCallback(dbus::Variant variant)
{
	std::cout << "Signal Received" << std::endl;
}

/*
TESTCASE(DbusRegisterObjectTest)
{
	ScopedGMainLoop mainloop;
	mainloop.dispatch([&](){
		klay::Latch nameAcquired;
		auto handler = [](const std::string& objectPath,
						  const std::string& interface,
						  const std::string& methodName,
						  dbus::Variant parameters) {
			if (objectPath != TESTSVC_OBJECT_PATH || interface != TESTSVC_INTERFACE) {
				throw klay::Exception("Unknown Method");
			}
			if (methodName == TESTSVC_METHOD_NOOP) {
				return dbus::Variant();
			} else if (methodName == TESTSVC_METHOD_PROCESS) {
				const gchar *arg = NULL;
				parameters.get("(&s)", &arg);
				return dbus::Variant("(s)", "result form process method");
			} else if (methodName == TESTSVC_METHOD_THROW) {
				int arg = 0;
				parameters.get("(i)", &arg);
				return dbus::Variant();
			}

			std::cout << "Unknown" << std::endl;
			return dbus::Variant();
		};

		try {
			dbus::Connection& svc = dbus::Connection::getSystem();
			svc.setName(TESTSVC_BUS_NAME, [&]{ nameAcquired.set(); },
										  []{});

			nameAcquired.wait();

			svc.registerObject(TESTSVC_OBJECT_PATH, manifest, handler, nullptr);
			svc.subscribeSignal("",
								TESTSVC_OBJECT_PATH,
								TESTSVC_INTERFACE,
								TESTSVC_SIGNAL_NOTIFY,
								signalCallback);

			std::cout << "Signal Test" << std::endl;
			dbus::Connection &client = dbus::Connection::getSystem();
			client.emitSignal(TESTSVC_BUS_NAME,
							  TESTSVC_OBJECT_PATH,
							  TESTSVC_INTERFACE,
							  TESTSVC_SIGNAL_NOTIFY,
							  "(s)",
							  "signal-data");

			std::cout << "Method Call Test" << std::endl;
			client.methodcall(TESTSVC_BUS_NAME,
							  TESTSVC_OBJECT_PATH,
							  TESTSVC_INTERFACE,
							  TESTSVC_METHOD_NOOP,
							  -1,
							  "()",
							  "()");


			const dbus::Variant& result = client.methodcall(TESTSVC_BUS_NAME,
															TESTSVC_OBJECT_PATH,
															TESTSVC_INTERFACE,
															TESTSVC_METHOD_PROCESS,
															-1,
															"(s)",
															"(s)", "arg");
			char *ret = NULL;
			result.get("(s)", &ret);
			std::cout << ">>> Result: " << ret << std::endl;
			client.methodcall(TESTSVC_BUS_NAME,
							  TESTSVC_OBJECT_PATH,
							  TESTSVC_INTERFACE,
							  TESTSVC_METHOD_THROW,
							  -1,
							  "()",
							  "(i)", 7);
		} catch (std::exception& e) {
			ERROR(KSINK, e.what());
		}
	});
}
*/

TESTCASE(DBusIntrospectionGetterTest)
{
	try {
		dbus::Introspection is(manifest);

		dbus::Interface interface = is.getInterface(TESTSVC_NOT_EXIST);
		TEST_EXPECT(true, interface == nullptr);

		interface = is.getInterface(TESTSVC_INTERFACE);
		TEST_EXPECT(true, interface != nullptr);

		dbus::Method method = is.getMethod(TESTSVC_INTERFACE, TESTSVC_NOT_EXIST);
		TEST_EXPECT(true, method == nullptr);

		method = is.getMethod(TESTSVC_INTERFACE, TESTSVC_METHOD_THROW);
		TEST_EXPECT(true, method != nullptr);

		dbus::Signal signal = is.getSignal(TESTSVC_INTERFACE, TESTSVC_NOT_EXIST);
		TEST_EXPECT(true, signal == nullptr);

		signal = is.getSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NOTIFY);
		TEST_EXPECT(true, signal != nullptr);

		signal = is.getSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NEW_NAME);
		TEST_EXPECT(true, signal == nullptr);

	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionDuplicatedInterface)
{
	try {
		dbus::Introspection is(manifest);
		is.addInterface(TESTSVC_INTERFACE);
		TEST_EXPECT(true, false);
	} catch (std::exception& e) {
		TEST_EXPECT(true, true);
	}
}

TESTCASE(DBusIntrospectionAddInterfaceAndMethod)
{
	try {
		dbus::Introspection is(manifest);
		dbus::Interface interface = is.getInterface(TESTSVC_INTERFACE_NEW_NAME);
		TEST_EXPECT(true, interface == nullptr);

		is.addInterface(TESTSVC_INTERFACE_NEW_NAME);

		interface = is.getInterface(TESTSVC_INTERFACE_NEW_NAME);
		TEST_EXPECT(true, interface != nullptr);

		dbus::Method method = is.getMethod(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_METHOD_NEW_NAME);
		TEST_EXPECT(true, method == nullptr);

		is.addMethod(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_METHOD_NEW_DATA);

		method = is.getMethod(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_METHOD_NEW_NAME);
		TEST_EXPECT(true, method != nullptr);

	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionAddMethod)
{
	try {
		dbus::Introspection is(manifest);
		dbus::Method method = is.getMethod(TESTSVC_INTERFACE, TESTSVC_METHOD_NEW_NAME);
		TEST_EXPECT(true, method == nullptr);

		is.addMethod(TESTSVC_INTERFACE, TESTSVC_METHOD_NEW_DATA);

		method = is.getMethod(TESTSVC_INTERFACE, TESTSVC_METHOD_NEW_NAME);
		TEST_EXPECT(true, method != nullptr);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionAddSignal)
{
	try {
		dbus::Introspection is(manifest);
		dbus::Signal signal = is.getSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NEW_NAME);
		TEST_EXPECT(true, signal == nullptr);

		is.addSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NEW_DATA);

		signal = is.getSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NEW_NAME);
		TEST_EXPECT(true, signal != nullptr);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionAddProperty)
{
	try {
		dbus::Introspection is(manifest);
		dbus::Property property = is.getProperty(TESTSVC_INTERFACE, TESTSVC_PROPERTY_NEW_NAME);
		TEST_EXPECT(true, property == nullptr);

		is.addProperty(TESTSVC_INTERFACE, TESTSVC_PROPERTY_NEW_DATA);

		property = is.getProperty(TESTSVC_INTERFACE, TESTSVC_PROPERTY_NEW_NAME);
		TEST_EXPECT(true, property != nullptr);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionAddTotal)
{
	try {
		dbus::Introspection is(manifest);
		is.addMethod(TESTSVC_INTERFACE, TESTSVC_METHOD_NEW_DATA);
		is.addSignal(TESTSVC_INTERFACE, TESTSVC_SIGNAL_NEW_DATA);
		is.addProperty(TESTSVC_INTERFACE, TESTSVC_PROPERTY_NEW_DATA);

		is.addInterface(TESTSVC_INTERFACE_NEW_NAME);
		is.addMethod(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_METHOD_NEW_DATA);
		is.addSignal(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_SIGNAL_NEW_DATA);
		is.addProperty(TESTSVC_INTERFACE_NEW_NAME, TESTSVC_PROPERTY_NEW_DATA);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusIntrospectionCheckDataFormat)
{
	try {
		dbus::Introspection is(manifest);
		is.addMethod(TESTSVC_INTERFACE, TESTSVC_WRONG_DATA_TYPE1);
		TEST_EXPECT(true, false);
	} catch (std::exception& e) {
		TEST_EXPECT(true, true);
	}
	try {
		dbus::Introspection is(manifest);
		is.addMethod(TESTSVC_INTERFACE, TESTSVC_WRONG_DATA_TYPE2);
		TEST_EXPECT(true, false);
	} catch (std::exception& e) {
		TEST_EXPECT(true, true);
	}
}

TESTCASE(DBusSignalAddToNotExistManifest)
{
	try {
		dbus::signal::Sender sender(TESTSVC_OBJECT_PATH, TESTSVC_INTERFACE_NEW_NAME);
		sender.addSignal(TESTSVC_MANIFEST_PATH, TESTSVC_SIGNAL_NEW_NAME, "(ss)");
		TEST_EXPECT(true, true);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusSignalAddToExistManifest)
{
	try {
		dbus::signal::Sender sender(TESTSVC_OBJECT_PATH, TESTSVC_INTERFACE_NEW_NAME);
		sender.addSignal(TESTSVC_MANIFEST_PATH, TESTSVC_SIGNAL_NEW_NAME, "(ss)");
		TEST_EXPECT(true, true);
	} catch (std::exception& e) {
		ERROR(KSINK, e.what());
		TEST_EXPECT(true, false);
	}
}

TESTCASE(DBusSignalEmitTest)
{
	{
		ScopedGMainLoop mainloop;
		mainloop.dispatch([&](){
			try {
				std::string manifest = dbus::Introspection::createXmlDataFromFile(TESTSVC_MANIFEST_PATH);
				dbus::Connection& svc = dbus::Connection::getSystem();
				svc.registerObject(TESTSVC_RUNTIME_OBJECT_PATH, manifest, nullptr, nullptr);

				std::string arg1 = "arg1";
				std::string arg2 = "arg2";
				auto onSignal = [&](dbus::Variant variant)
					{
						char *ret1 = NULL;
						char *ret2 = NULL;
						variant.get("(ss)", &ret1, &ret2);

						std::cout << ">>>>" << ret1 << ret2 << std::endl;

// [TODO] Prevent abort if TEST_EXPECT called
//						TEST_EXPECT(true, arg1.compare(retStr1) == 0);
//						TEST_EXPECT(true, arg2.compare(retStr2) == 0);
					};

				dbus::signal::Receiver receiver(TESTSVC_RUNTIME_OBJECT_PATH, TESTSVC_INTERFACE_NEW_NAME);
				receiver.subscribe(TESTSVC_SIGNAL_NEW_NAME, onSignal);

				dbus::signal::Sender sender(TESTSVC_RUNTIME_OBJECT_PATH, TESTSVC_INTERFACE_NEW_NAME);
				sender.emit(TESTSVC_SIGNAL_NEW_NAME, "(ss)", arg1.c_str(), arg2.c_str());

				TEST_EXPECT(true, true);
			} catch (std::exception& e) {
				ERROR(KSINK, e.what());
				TEST_EXPECT(true, false);
			}
		});

	}
}
