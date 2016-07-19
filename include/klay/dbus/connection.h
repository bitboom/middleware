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

#include <klay/dbus/variant.h>

namespace dbus {

class Connection {
public:
	typedef unsigned int subscriptionId;
	typedef std::function<void(Variant)> signalCallback;

	Connection() = delete;
	Connection(const Connection&) = delete;
	~Connection();

	Connection& operator=(const Connection&) = delete;

	static Connection& getSystem();

	subscriptionId subscribeSignal(const std::string& sender,
								   const std::string& interface,
								   const std::string& object,
								   const std::string& member,
								   const signalCallback callback);


	void unsubscribeSignal(subscriptionId id);

	const Variant methodcall(const std::string& busName,
							 const std::string& object,
							 const std::string& interface,
							 const std::string& method,
							 int timeout,
							 const std::string& replyType,
							 const std::string& paramType,
							 ...);

private:
	Connection(const std::string& address);

	GDBusConnection* connection;
};

} // namespace dbus

 #endif //! __RUNTIME_DBUS_CONNECTION_H__
