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

#include <klay/exception.h>
#include <klay/audit/logger.h>
#include <klay/dbus/variant.h>
#include <klay/dbus/connection.h>
#include <klay/latch.h>

#include <klay/testbench.h>

const std::string TESTSVC_BUS_NAME       = "org.tizen.klay";
const std::string TESTSVC_OBJECT_PATH    = "/org/tizen/klay";
const std::string TESTSVC_INTERFACE      = "test.api";
const std::string TESTSVC_METHOD_NOOP    = "Noop";
const std::string TESTSVC_METHOD_PROCESS = "Process";
const std::string TESTSVC_METHOD_THROW   = "Throw";
const std::string TESTSVC_SIGNAL_NOTIFY  = "Notify";

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
	"			<arg type='s' name='arument'/>"
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

class ScopedGMainLoop {
public:
	ScopedGMainLoop() :
		mainloop(g_main_loop_new(NULL, FALSE), g_main_loop_unref)
	{
		handle = std::thread(g_main_loop_run, mainloop.get());
	}

	~ScopedGMainLoop()
	{
		while (!g_main_loop_is_running(mainloop.get())) {
			std::this_thread::yield();
		}

		g_main_loop_quit(mainloop.get());
		handle.join();
	}

private:
	std::unique_ptr<GMainLoop, void(*)(GMainLoop*)> mainloop;
	std::thread handle;
};

void signalCallback(dbus::Variant variant)
{
	std::cout << "Signal Received" << std::endl;
}

TESTCASE(DbusRegisterObjectTest)
{
	runtime::Latch nameAcquired;
	ScopedGMainLoop mainloop;

    auto handler = [](const std::string& objectPath,
                      const std::string& interface,
                      const std::string& methodName,
                      dbus::Variant parameters) {
        if (objectPath != TESTSVC_OBJECT_PATH || interface != TESTSVC_INTERFACE) {
			throw runtime::Exception("Unknown Method");
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
		ERROR(e.what());
	}
}
