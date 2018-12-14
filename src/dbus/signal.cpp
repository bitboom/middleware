/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <klay/dbus/signal.h>
#include <klay/dbus/introspection.h>
#include <klay/exception.h>

namespace klay {
namespace dbus {
namespace signal {

Sender::Sender(const std::string &objectPath, const std::string &interfaceName) :
	busName(""), objectPath(objectPath), interfaceName(interfaceName)
{
}

void Sender::addSignal(const std::string &manifestPath,
					   const std::string &signalName,
					   const std::string &argumentType) const
{
	std::string xmlData = Introspection::createXmlDataFromFile(manifestPath);
	Introspection introspect(xmlData);
	if (introspect.getInterface(interfaceName) == nullptr)
		introspect.addInterface(interfaceName);

	if (introspect.getSignal(interfaceName, signalName) == nullptr)
		introspect.addSignal(interfaceName, signalName, argumentType);
	else
		return;

	Introspection::writeXmlDataToFile(manifestPath, introspect.getXmlData());
}

void Sender::setBusName(const std::string &name)
{
	busName = name;
}

void Sender::setInterfaceName(const std::string &name)
{
	interfaceName = name;
}

Receiver::Receiver(const std::string &objectPath, const std::string &interfaceName) :
	objectPath(objectPath), interfaceName(interfaceName), senderName("")
{
}

void Receiver::setInterfaceName(const std::string &name)
{
	interfaceName = name;
}

void Receiver::setSenderName(const std::string &name)
{
	senderName = name;
}

unsigned int Receiver::subscribe(const std::string &signalName,
								 const SignalCallback& signalCallback) const
{
	dbus::Connection &conn = dbus::Connection::getSystem();
	return conn.subscribeSignal(senderName,
								objectPath,
								interfaceName,
								signalName,
								signalCallback);
}

void Receiver::unsubscribe(unsigned int id) const
{
	dbus::Connection &conn = dbus::Connection::getSystem();
	return conn.unsubscribeSignal(id);
}

} // namespace signal
} // namespace dbus
} // namespace klay
