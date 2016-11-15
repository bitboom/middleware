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

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <iostream>
#include <stdexcept>

#include <klay/exception.h>
#include <klay/xml/parser.h>
#include <klay/xml/document.h>
#include <klay/audit/logger.h>

#include <klay/testbench.h>

const std::string testXmlFilePath = TEST_DATA_DIR "/sample-policy.xml";

TESTCASE(XPath)
{
	try {
		xml::Document* document = xml::Parser::parseFile(testXmlFilePath);
		xml::Node::NodeList nodes = document->evaluate("//policy-group[@name='APPLICATION']/policy[@name='SET_APP_INSTALLATION_MODE']");
		xml::Node::NodeList::iterator iter = nodes.begin();

		TEST_EXPECT(false, iter == nodes.end());

		while (iter != nodes.end()) {
			++iter;
		}
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(XmlDomTree)
{
	try {
		xml::Document* document = xml::Parser::parseFile(testXmlFilePath);

		xml::Node& root = document->getRootNode();

		xml::Node::NodeList list = root.getChildren();
		xml::Node::NodeList::iterator iter = list.begin();

		TEST_EXPECT(false, iter == list.end());

		while (iter != list.end()) {
			++iter;
		}
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(XmlGenerate)
{
	try {
		xml::Document doc("TestNode", "0.1");
		xml::Node &root = doc.getRootNode();
		xml::Node node = root.addNewChild("ChildNode");
		TEST_EXPECT("ChildNode", node.getName());

		node.setName("ModifiedChildNode");
		TEST_EXPECT("ModifiedChildNode", node.getName());

		node.setContent("Content");
		TEST_EXPECT("Content", node.getContent());

		node.setProp("Prop", "Value");
		TEST_EXPECT("Value", node.getProp("Prop"));

		doc.write("/tmp/test.xml", "UTF-8", true);
	} catch (runtime::Exception& e) {
		TEST_FAIL(e.what());
	}
}

TESTCASE(XmlException)
{
	try {
		xml::Parser::parseFile("Invalid Source");
	} catch (runtime::Exception& e) {
	}
}
