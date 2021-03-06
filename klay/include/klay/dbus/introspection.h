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
#include <vector>
#include <utility>

#include <klay/klay.h>

namespace klay {
namespace dbus {

using BusNode = GDBusNodeInfo*;
using Interface = GDBusInterfaceInfo*;
using Method = GDBusMethodInfo*;
using Signal = GDBusSignalInfo*;
using Property = GDBusPropertyInfo*;

using XmlProperties = std::vector<std::pair<std::string, std::string>>;

class KLAY_EXPORT Introspection {
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
	static std::string createXmlDataFromFile(const std::string &path);
	static void writeXmlDataToFile(const std::string &path, const std::string &xmlData);

	void addInterface(const std::string &name);
	void addMethod(const std::string &interfaceName, const std::string &methodData);
	void addSignal(const std::string &interfaceName, const std::string &signalData);
	void addProperty(const std::string &interfaceName, const std::string &propertyData);

	void addSignal(const std::string &interfaceName,
				   const std::string &signalName,
				   const std::string &argumentType);

private:
	BusNode getBusNode(const std::string &xmlData);
	void update(void);

	std::string getXmlBeginTag(const std::string &node,
							   const XmlProperties &properties) const;
	std::string getXmlEndTag(const std::string &node) const;

	std::string parseXmlProperties(const XmlProperties &properties) const;

	void addInternalData(const std::string &interfaceName, const std::string &data);

	void checkDataFormat(const std::string &data) const;

	BusNode busNode;
	std::string xmlData;
};

} // namespace dbus
} // namespace klay

namespace dbus = klay::dbus;

#endif //! __RUNTIME_DBUS_INTROSPECTION_H__
