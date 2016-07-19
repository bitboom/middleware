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

#ifndef __XML_DOCUMENT_H__
#define __XML_DOCUMENT_H__

#include <libxml/tree.h>
#include <libxml/parser.h>

#include <string>

#include <klay/xml/node.h>

namespace xml {

class Document {
public:
	Document(const std::string& root, const std::string& version = XML_DEFAULT_VERSION);
	Document(xmlDoc* doc);

	~Document();

	Node& getRootNode();

	Node::NodeList evaluate(const std::string& xpath);
	void write(const std::string& filename, const std::string& encoding, bool formatted);

private:
	Node* rootNode;
	xmlDoc* implementation;
};

} // namespace xml
#endif //__XML_DOCUMENT_H__
