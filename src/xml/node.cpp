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
#include <klay/xml/node.h>

namespace xml {

Node::Node(xmlNode* node) :
	implementation(node)
{
}

Node::Node(Node&& node) :
	implementation(node.implementation)
{
}

Node::~Node()
{
}

Node::NodeList Node::getChildren() const
{
	NodeList nodeList;

	auto child = implementation->xmlChildrenNode;
	while (child != nullptr) {
		nodeList.emplace_back(child);
		child = child->next;
	}

	return nodeList;
}

Node Node::addNewChild(const std::string& name)
{
	xmlNode* nodePtr = xmlNewNode(NULL, xmlStrdup((const xmlChar*)name.c_str()));
	if (nodePtr == nullptr) {
		throw runtime::Exception("Can not create a new node");
	}
	xmlAddChild(implementation, nodePtr);

	return Node(nodePtr);
}

std::string Node::getName() const
{
	return implementation->name ? (const char*)implementation->name : "";
}

void Node::setName(const std::string& name)
{
	xmlNodeSetName(implementation, (const xmlChar*)name.c_str());
}

std::string Node::getContent() const
{
	xmlChar* content = xmlNodeGetContent(implementation);
	if (content == NULL) {
		return "";
	}
	std::string ret((const char*)content);
	xmlFree(content);
	return ret;
}

void Node::setContent(const std::string& content)
{
	xmlNodeSetContent(implementation, (xmlChar*)content.c_str());
}

std::string Node::getProp(const std::string& name) const
{
	if (implementation->type != XML_ELEMENT_NODE) {
		throw runtime::Exception("This node type does not have properties");
	}

	xmlChar* prop = xmlGetProp(implementation, (xmlChar*)name.c_str());
	if (prop) {
		std::string ret((const char*)prop);
		xmlFree(prop);
		return ret;
	}

	return "";
}

void Node::setProp(const std::string& name, const std::string& val)
{
	if (implementation->type != XML_ELEMENT_NODE) {
		throw runtime::Exception("Can not set properties for this node type");
	}

	xmlSetProp(implementation, (xmlChar*)name.c_str(), (xmlChar*)val.c_str());
}

bool Node::isBlank() const
{
	return xmlIsBlankNode(const_cast<xmlNode*>(implementation));
}

} // namespace xml
