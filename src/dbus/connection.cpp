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

template <typename T>
void freeUserData(void *userData)
{
	delete reinterpret_cast<T *>(userData);
}

template<typename T>
const T& userDataCast(void *userData)
{
	return *reinterpret_cast<T *>(userData);
}

} // namespace

Connection::Connection(const std::string& address) :
	 connection(nullptr), ownedNameId(0)
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
	for (auto client : watchedClients) {
		g_bus_unwatch_name(client.second);
	}

	if (ownedNameId) {
		g_bus_unown_name(ownedNameId);
	}

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

Connection::SubscriptionId Connection::subscribeSignal(const std::string& sender,
													   const std::string& interface,
													   const std::string& object,
													   const std::string& member,
													   const SignalCallback& callback)
{
	return g_dbus_connection_signal_subscribe(connection,
											  sender.empty()    ? NULL : sender.c_str(),
											  interface.empty() ? NULL : interface.c_str(),
											  object.empty()    ? NULL : object.c_str(),
											  member.empty()    ? NULL : member.c_str(),
											  NULL,
											  G_DBUS_SIGNAL_FLAGS_NONE,
											  &onSignal,
											  new SignalCallback(callback),
											  &freeUserData<SignalCallback>);
}

void Connection::unsubscribeSignal(Connection::SubscriptionId id)
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
	GVariant* variant = g_variant_new_va(paramType.c_str(), NULL, &ap);
	va_end(ap);

	result = g_dbus_connection_call_sync(connection,
										 busName.empty() ? NULL : busName.c_str(),
										 object.c_str(),
										 interface.c_str(),
										 method.c_str(),
										 paramType.empty() ? NULL : variant,
										 replyType.empty() ? NULL : G_VARIANT_TYPE(replyType.c_str()),
										 G_DBUS_CALL_FLAGS_NONE,
										 timeout,
										 NULL,
										 &error);

	if (error) {
		ERROR(error->message);
		throw runtime::Exception(error->message);
	}

	return result;
}

void Connection::setName(const std::string& name,
						 const VoidCallback& nameAcquiredCallback,
						 const VoidCallback& nameLostCallback)
{
	ownedNameId = g_bus_own_name_on_connection(connection,
											   name.c_str(),
											   G_BUS_NAME_OWNER_FLAGS_NONE,
											   &onNameAcquired,
											   &onNameLost,
											   new NameCallback(nameAcquiredCallback, nameLostCallback),
											   &freeUserData<NameCallback>);
}

Connection::ObjectId Connection::registerObject(const std::string& object,
											    const std::string& manifest,
												const MethodCallCallback& methodcall,
												const ClientVanishedCallback& vanished)
{
	Error error;
	GDBusNodeInfo* node = g_dbus_node_info_new_for_xml(manifest.c_str(), &error);
	if (node != NULL && (node->interfaces == NULL ||
						 node->interfaces[0] == NULL ||
						 node->interfaces[1] != NULL)) {
		g_dbus_node_info_unref(node);
		g_set_error(&error, G_MARKUP_ERROR, G_MARKUP_ERROR_INVALID_CONTENT,
							"Unexpected interface");
	}

	if (error) {
		throw runtime::Exception(error->message);
	}

	GDBusInterfaceInfo* inf = node->interfaces[0];
	GDBusInterfaceVTable vtable;
	vtable.method_call = &Connection::onMethodCall;
	vtable.get_property = NULL;
	vtable.set_property = NULL;

	ObjectId id = g_dbus_connection_register_object(connection,
												    object.c_str(),
												    inf,
												    &vtable,
												    new MethodCallback(methodcall, vanished, this),
												    &freeUserData<MethodCallback>,
												    &error);
	g_dbus_node_info_unref(node);
    if (error) {
        ERROR(error->message);
        throw runtime::Exception(error->message);
    }

	return id;
}

void Connection::unregisterObject(ObjectId id)
{
	g_dbus_connection_unregister_object(connection, id);
}

void Connection::onClientVanish(GDBusConnection* connection, const gchar* name, gpointer userData)
{
	DEBUG("Client Vanished:" << name);
	const VanishedCallback& callback = userDataCast<VanishedCallback>(userData);

	guint id = callback.watchedClients[name];
	callback.watchedClients.erase(name);

	if (callback.clientVanished) {
		callback.clientVanished(name);
	}

	g_bus_unwatch_name(id);
}

void Connection::onNameAcquired(GDBusConnection* connection, const gchar* name, gpointer userData)
{
	DEBUG("Name Acquired: " << name);
	const NameCallback& callbacks = userDataCast<NameCallback>(userData);
	if (callbacks.nameAcquired) {
		callbacks.nameAcquired();
	}
}

void Connection::onNameLost(GDBusConnection* connection, const gchar* name, gpointer userData)
{
	DEBUG("Name Lost" << name);
	const NameCallback& callbacks = userDataCast<NameCallback>(userData);
	if (callbacks.nameLost) {
		callbacks.nameLost();
	}
}

void Connection::onSignal(GDBusConnection* connection,
						  const gchar *sender,
						  const gchar *objectPath,
						  const gchar *interface,
						  const gchar *signal,
						  GVariant *parameters,
						  gpointer userData)
{
	DEBUG("Signal : " << sender << " : " << objectPath << " : "
					  << interface << " : " << signal);
	const SignalCallback& callback = userDataCast<SignalCallback>(userData);
	if (callback) {
		callback(Variant(parameters));
	}
}

void Connection::onMethodCall(GDBusConnection* connection,
							  const gchar* sender,
							  const gchar* objectPath,
							  const gchar* interface,
							  const gchar* method,
							  GVariant* parameters,
							  GDBusMethodInvocation* invocation,
							  gpointer userData)
{
	DEBUG("MethodCall: " << sender << " : " << objectPath << " : "
						 << interface << " : " << method);
	const MethodCallback callbackSet = userDataCast<MethodCallback>(userData);

	ClientMap &watchMap = callbackSet.connection->watchedClients;

	if (watchMap.find(sender) == watchMap.end()) {
		guint id = g_bus_watch_name_on_connection(connection,
												  sender,
												  G_BUS_NAME_WATCHER_FLAGS_NONE,
												  NULL,
												  &onClientVanish,
												  new VanishedCallback(callbackSet.clientVanished, watchMap),
												  &freeUserData<VanishedCallback>);
		watchMap[sender] = id;
	}

	try {
		Variant variant = callbackSet.methodcall(objectPath, interface, method, Variant(parameters));
		g_dbus_method_invocation_return_value(invocation, &variant);
	} catch (runtime::Exception& e) {
		ERROR("Error on metod handling");
		g_dbus_method_invocation_return_dbus_error(invocation, (interface + std::string(".Error")).c_str(), e.what());
	}
}

} // namespace dbus
