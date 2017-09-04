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

#ifndef __RUNTIME_DBUS_INTROSPECTION_H__
#define __RUNTIME_DBUS_INTROSPECTION_H__

#include <gio/gio.h>

#include <string>

namespace dbus {

using BusNode = GDBusNodeInfo*;
using Interface = GDBusInterfaceInfo*;
using Method = GDBusMethodInfo*;
using Signal = GDBusSignalInfo*;
using Property = GDBusPropertyInfo*;

class Introspection {
public:
	explicit Introspection(const std::string &xmlData);
	virtual ~Introspection(void);

	Introspection(const Introspection &) = delete;
	Introspection(Introspection &&) = delete;
	Introspection &operator=(const Introspection &) = delete;
	Introspection &operator=(Introspection &&) = delete;

	Interface getInterface(const std::string &name) const;
	Method getMethod(const std::string &interfaceName,
					 const std::string &methodName) const;
	Signal getSignal(const std::string &interfaceName,
					 const std::string &signalName) const;
	Property getProperty(const std::string &interfaceName,
						 const std::string &propertyName) const;

	std::string getXmlData(unsigned int indent = 0);

	void addInterface(const std::string &name);
	void addMethod(const std::string &interfaceName, const std::string &methodData);
	void addSignal(const std::string &interfaceName, const std::string &signalData);
	void addProperty(const std::string &interfaceName, const std::string &propertyData);

private:
	BusNode getBusNode(const std::string &xmlData);
	void update(void);

	std::string getInterfaceBeginTag(const std::string &name) const;
	std::string getInterfaceEndTag(void) const;

	void addInternalData(const std::string &interfaceName, const std::string &data);

	void checkDataFormat(const std::string &data) const;

	BusNode busNode;
	std::string xmlData;
};

} // namespace dbus

#endif //! __RUNTIME_DBUS_INTROSPECTION_H__
