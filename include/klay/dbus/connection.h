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

#ifndef __RUNTIME_DBUS_CONNECTION_H__
#define __RUNTIME_DBUS_CONNECTION_H__

#include <gio/gio.h>

#include <string>
#include <functional>
#include <map>

#include <klay/dbus/variant.h>

namespace dbus {

typedef std::function<void(Variant parameters)> SignalCallback;

class Connection {
public:
	typedef unsigned int NameId;
	typedef unsigned int ObjectId;
	typedef unsigned int SubscriptionId;

	typedef std::function<void()> VoidCallback;


	typedef std::function<void(const std::string& name)> ClientVanishedCallback;

	typedef std::function<Variant(const std::string& objectPath,
								  const std::string& interface,
								  const std::string& method,
								  Variant parameters)> MethodCallCallback;

	Connection(const std::string& address);

	Connection() = delete;
	Connection(const Connection&) = delete;
	~Connection();

	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = delete;

	static Connection& getSystem();

	void setName(const std::string& name, const VoidCallback& nameAcquireCallback,
										  const VoidCallback& nameLostCallback);

	SubscriptionId subscribeSignal(const std::string& sender,
								   const std::string& object,
								   const std::string& interface,
								   const std::string& member,
								   const SignalCallback& callback);

	void unsubscribeSignal(SubscriptionId id);

	Variant methodcall(const std::string& busName,
					   const std::string& object,
					   const std::string& interface,
					   const std::string& method,
					   int timeout,
					   const std::string& replyType,
					   const std::string& paramType,
					   ...);

	void emitSignal(const std::string& busName,
					const std::string& object,
					const std::string& interface,
					const std::string& name,
					const std::string& paramType,
					...);

	ObjectId registerObject(const std::string& interface,
							const std::string& manifest,
							const MethodCallCallback& methodcall,
							const ClientVanishedCallback& vanished);

	void unregisterObject(ObjectId id);

private:

	static void onNameAcquired(GDBusConnection* connection,
							   const gchar* name, gpointer userData);

	static void onNameLost(GDBusConnection* connection,
						   const gchar* name, gpointer userData);

	static void onClientVanish(GDBusConnection* connection,
							   const gchar* name, gpointer userData);

	static void onSignal(GDBusConnection* connection,
						 const gchar *sender,
						 const gchar *objectPath,
						 const gchar *interface,
						 const gchar *signal,
						 GVariant *parameters,
						 gpointer userData);

	static void onMethodCall(GDBusConnection* connection,
							 const gchar* sender,
							 const gchar* objectPath,
							 const gchar* interface,
							 const gchar* method,
							 GVariant* parameters,
							 GDBusMethodInvocation* invocation,
							 gpointer userData);

private:
	typedef std::map<std::string, guint> ClientMap;

	struct NameCallback {
		VoidCallback nameAcquired;
		VoidCallback nameLost;

		NameCallback(const VoidCallback& acquired, const VoidCallback& lost) :
			nameAcquired(acquired), nameLost(lost)
		{
		}
	};

	struct VanishedCallback {
		ClientVanishedCallback clientVanished;
		ClientMap &watchedClients;

		VanishedCallback(const ClientVanishedCallback& vanished, ClientMap& clients) :
			clientVanished(vanished), watchedClients(clients)
		{
		}
	};

	struct MethodCallback {
		MethodCallCallback methodcall;
		ClientVanishedCallback clientVanished;
		Connection* connection;

		MethodCallback(const MethodCallCallback& method,
					   const ClientVanishedCallback& vanished,
					   Connection* conn) :
			methodcall(method), clientVanished(vanished), connection(conn)
		{
		}
	};

	ClientMap watchedClients;
	GDBusConnection* connection;
	NameId ownedNameId;
};

} // namespace dbus

 #endif //! __RUNTIME_DBUS_CONNECTION_H__
