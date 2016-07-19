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

#ifndef __XML_NODE_H__
#define __XML_NODE_H__

#include <string>
#include <vector>

#include <libxml/tree.h>
#include <libxml/parser.h>

namespace xml {

class Node {
public:
	typedef std::vector<Node> NodeList;

	explicit Node(xmlNode* node);
	Node(Node&&);
	Node(const Node&) = delete;

	~Node();

	Node& operator=(const Node&) = delete;

	NodeList getChildren() const;
	Node addNewChild(const std::string& name);

	std::string getName() const;
	void setName(const std::string& name);

	std::string getContent() const;
	void setContent(const std::string& content);

	std::string getProp(const std::string& name) const;
	void setProp(const std::string& name, const std::string& val);

	bool isBlank() const;

private:
	xmlNode* implementation;
};

} // namespace xml
#endif //__XML_NODE_H__
