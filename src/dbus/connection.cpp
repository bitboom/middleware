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
#include <klay/exception.h>
#include <klay/dbus/error.h>
#include <klay/dbus/variant.h>
#include <klay/dbus/connection.h>
#include <klay/audit/logger.h>

namespace dbus {

namespace {

const std::string DBUS_SYSTEM_BUS_ADDRESS = "kernel:path=/sys/fs/kdbus/0-system/bus;unix:path=/var/run/dbus/system_bus_socket";

void defaultCallback(GDBusConnection* connection,
					 const gchar *sender_name,
					 const gchar *object_path,
					 const gchar *interface_name,
					 const gchar *signal_name,
					 GVariant *parameters,
					 gpointer user_data) {
	Connection::signalCallback* func = reinterpret_cast<Connection::signalCallback*> (user_data);
	(*func)(Variant(parameters));
	delete func;
}

} // namespace

Connection::Connection(const std::string& address) :
	 connection(nullptr)
{
	Error error;
	const GDBusConnectionFlags flags = static_cast<GDBusConnectionFlags>
		(G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT | G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION);

	connection = g_dbus_connection_new_for_address_sync(address.c_str(), flags, NULL, NULL, &error);
	if (error) {
		ERROR(error->message);
		throw runtime::Exception(error->message);
	}
}

Connection::~Connection()
{
	if (connection) {
		g_dbus_connection_close_sync(connection, NULL, NULL);
		g_object_unref(connection);
	}
}

Connection& Connection::getSystem()
{
	static Connection __instance__(DBUS_SYSTEM_BUS_ADDRESS);
	return __instance__;
}

Connection::subscriptionId Connection::subscribeSignal(const std::string& sender,
													   const std::string& interface,
													   const std::string& object,
													   const std::string& member,
													   const signalCallback callback)
{
	subscriptionId id;
	id = g_dbus_connection_signal_subscribe(connection,
											sender.empty()    ? NULL : sender.c_str(),
											interface.empty() ? NULL : interface.c_str(),
											object.empty()    ? NULL : object.c_str(),
											member.empty()    ? NULL : member.c_str(),
											NULL,
											G_DBUS_SIGNAL_FLAGS_NONE,
											defaultCallback,
											new signalCallback(callback),
											NULL);
	return id;
}

void Connection::unsubscribeSignal(Connection::subscriptionId id)
{
	g_dbus_connection_signal_unsubscribe(connection, id);
}

const Variant Connection::methodcall(const std::string& busName,
									 const std::string& object,
									 const std::string& interface,
									 const std::string& method,
									 int timeout,
									 const std::string& replyType,
									 const std::string& paramType,
									 ...)
{
	Variant result;
	Error error;
	va_list ap;

	va_start(ap, paramType);
	result = g_dbus_connection_call_sync(connection,
										 busName.empty() ? NULL :
										 busName.c_str(),
										 object.c_str(),
										 interface.c_str(),
										 method.c_str(),
										 paramType.empty() ? NULL :
										 g_variant_new_va(paramType.c_str(), NULL, &ap),
										 replyType.empty() ? NULL :
										 G_VARIANT_TYPE(replyType.c_str()),
										 G_DBUS_CALL_FLAGS_NONE,
										 timeout,
										 NULL,
										 &error);
	va_end(ap);

	if (error) {
		ERROR(error->message);
		throw runtime::Exception(error->message);
	}

	return result;
}

} // namespace dbus
