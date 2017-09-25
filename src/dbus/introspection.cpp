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

#include <klay/dbus/introspection.h>
#include <klay/dbus/error.h>
#include <klay/filesystem.h>
#include <klay/exception.h>

#include <unistd.h>
#include <fcntl.h>

#include <memory>
#include <functional>

namespace dbus {

namespace {

const std::string MANIFEST_TEMPLATE = "<node></node>";

const std::string INTERFACE_NODE = "interface";
const std::string SIGNAL_NODE = "signal";
const std::string ARGUMENT_NODE = "arg";

}

Introspection::Introspection(const std::string &xmlData) :
	busNode(getBusNode(xmlData)), xmlData(getXmlData())
{
}

Introspection::~Introspection(void)
{
	if (busNode)
		::g_dbus_node_info_unref(busNode);
}

BusNode Introspection::getBusNode(const std::string &xmlData)
{
	if (xmlData.empty())
		throw runtime::Exception("Invalid argument.");

	dbus::Error error;
	auto busNode = ::g_dbus_node_info_new_for_xml(xmlData.c_str(), &error);
	if (busNode == nullptr || error)
		throw runtime::Exception("Failed to get BusNode.");

	return busNode;
}

Interface Introspection::getInterface(const std::string &name) const
{
	if (name.empty())
		throw runtime::Exception("Invalid argument.");

	return ::g_dbus_node_info_lookup_interface(busNode, name.c_str());
}

Method Introspection::getMethod(const std::string &interfaceName,
								const std::string &methodName) const
{
	auto interface = getInterface(interfaceName);
	if (interface == nullptr || methodName.empty())
		throw runtime::Exception("Invalid argument.");

	return ::g_dbus_interface_info_lookup_method(interface, methodName.c_str());
}

Signal Introspection::getSignal(const std::string &interfaceName,
								const std::string &signalName) const
{
	auto interface = getInterface(interfaceName);
	if (interface == nullptr || signalName.empty())
		throw runtime::Exception("Invalid argument.");

	return ::g_dbus_interface_info_lookup_signal(interface, signalName.c_str());
}

Property Introspection::getProperty(const std::string &interfaceName,
									const std::string &propertyName) const
{
	auto interface = getInterface(interfaceName);
	if (interface == nullptr || propertyName.empty())
		throw runtime::Exception("Invalid argument.");

	return ::g_dbus_interface_info_lookup_property(interface, propertyName.c_str());
}

std::string Introspection::getXmlData(unsigned int indent)
{
	using ScopedGString = std::unique_ptr<GString, std::function<void(GString*)>>;
	ScopedGString buf(g_string_new(""), [](GString *ptr)
		{
			::g_string_free(ptr, TRUE);
		});
	if (buf == nullptr)
		throw runtime::Exception("Out of memory.");

	::g_dbus_node_info_generate_xml(busNode, indent, buf.get());
	return std::string(buf->str);
}

std::string Introspection::createXmlDataFromFile(const std::string &path)
{
	runtime::File manifest(path);
	if (!manifest.exists()) {
		manifest.create(644);
		manifest.lock();
		manifest.write(MANIFEST_TEMPLATE.c_str(), MANIFEST_TEMPLATE.length());
		manifest.unlock();
	}

	manifest.open(O_RDONLY);
	std::vector<char> buf(manifest.size() + 1);
	manifest.read(buf.data(), manifest.size());
	return std::string(buf.data());
}

void Introspection::writeXmlDataToFile(const std::string &path,
									   const std::string &xmlData)
{
	runtime::File manifest(path);
	manifest.open(O_WRONLY | O_TRUNC);
	manifest.lock();
	manifest.write(xmlData.c_str(), xmlData.length());
	manifest.unlock();
}

void Introspection::addInterface(const std::string &name)
{
	if (getInterface(name) != nullptr)
		throw runtime::Exception("Interface is already exist:" + name);

	std::size_t offset = xmlData.find("</node>");
	if (offset == std::string::npos)
		throw runtime::Exception("Failed to find </node>.");

	XmlProperties properties;
	properties.emplace_back(std::make_pair("name", name));

	xmlData.insert(offset, getXmlBeginTag(INTERFACE_NODE, properties) +
						   getXmlEndTag(INTERFACE_NODE));
	update();
}

std::string Introspection::parseXmlProperties(const XmlProperties &properties) const
{
	std::string parsed = "";
	for (const auto &property : properties)
		parsed.append(property.first + "=\"" + property.second + "\" ");

	return parsed.substr(0, parsed.length() - 1);
};

std::string Introspection::getXmlBeginTag(const std::string &node,
										  const XmlProperties &properties) const
{
	const std::string nameToken = "@NODE@";
	const std::string propertyToken = "@PROPERTIES@";
	std::string tagTemplate = "<@NODE@ @PROPERTIES@>";

	std::string parsed = parseXmlProperties(properties);
	tagTemplate.replace(tagTemplate.find(nameToken), nameToken.length(), node);
	tagTemplate.replace(tagTemplate.find(propertyToken), propertyToken.length(), parsed);

	return tagTemplate;
}

std::string Introspection::getXmlEndTag(const std::string &node) const
{
	return std::string("</" + node + ">");
}

// TODO: Check more strict.
void Introspection::checkDataFormat(const std::string &data) const
{
	if (data.empty() || data.length() < 3)
		throw runtime::Exception("Invalid argument.");

	const std::string beginChar = "<";
	if (data.compare(0, beginChar.length(), beginChar) != 0)
		throw runtime::Exception("Xml data should be begin as: " + beginChar);

	const std::string endChar = "/>";
	if (data.compare(data.length() - endChar.length(), endChar.length(), endChar) != 0 &&
		data.find("/") == std::string::npos)
		throw runtime::Exception("Xml data should be contain '/' or end as: " + endChar);
}

void Introspection::addMethod(const std::string &interfaceName,
							  const std::string &methodData)
{
	addInternalData(interfaceName, methodData);
}

void Introspection::addSignal(const std::string &interfaceName,
							  const std::string &signalName,
							  const std::string &argumentType)
{
	XmlProperties properties;
	properties.emplace_back(std::make_pair("name", signalName));
	std::string xmlData = getXmlBeginTag(SIGNAL_NODE, properties);

	int index = 1;
	for (const auto &type : argumentType) {
		if (type == '(' || type == ')')
			continue;

		properties.clear();
		properties.emplace_back(std::make_pair("type", std::string(1, type)));
		properties.emplace_back(std::make_pair("name",
											   "argument" + std::to_string(index++)));

		xmlData.append(getXmlBeginTag(ARGUMENT_NODE, properties));
		xmlData.append(getXmlEndTag(ARGUMENT_NODE));
	}

	xmlData.append(getXmlEndTag(SIGNAL_NODE));

	addInternalData(interfaceName, xmlData);
}

void Introspection::addSignal(const std::string &interfaceName,
							  const std::string &signalData)
{
	addInternalData(interfaceName, signalData);
}

void Introspection::addProperty(const std::string &interfaceName,
								const std::string &propertyData)
{
	addInternalData(interfaceName, propertyData);
}

void Introspection::addInternalData(const std::string &interfaceName,
									const std::string &data)
{
	checkDataFormat(data);

	XmlProperties properties;
	properties.emplace_back(std::make_pair("name", interfaceName));

	std::string iTemplate = getXmlBeginTag(INTERFACE_NODE, properties);
	std::size_t offset = xmlData.find(iTemplate);
	if (offset == std::string::npos)
		throw runtime::Exception("Failed to find interface xml node: " + interfaceName);

	xmlData.insert(offset + iTemplate.length() + 1, data);
	update();
}

void Introspection::update(void)
{
	if (busNode)
		::g_dbus_node_info_unref(busNode);

	busNode = getBusNode(xmlData);
	xmlData = getXmlData();
}

} // namespace dbus
