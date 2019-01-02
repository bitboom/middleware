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
#include <libxml/parserInternals.h>

#include <klay/exception.h>
#include <klay/xml/parser.h>
#include <klay/xml/keepblanks.h>

namespace klay {
namespace xml {

Document* Parser::parseContext(xmlParserCtxt* context, bool validate)
{
	if (context == nullptr) {
		throw klay::Exception("Could not create parser context");
	}

	KeepBlanks(false);

	int options = 0;

	if (validate) {
		options |= XML_PARSE_DTDVALID;
	} else {
		options &= ~XML_PARSE_DTDVALID;
	}

	xmlCtxtUseOptions(context, options);

	if (xmlParseDocument(context) < 0) {
		xmlFreeParserCtxt(context);
		throw klay::Exception("Parsing failed");
	}

	xmlDoc* document = context->myDoc;

	// We took the ownership on the doc
	context->myDoc = nullptr;

	xmlFreeParserCtxt(context);

	return new Document(document);
}

Document* Parser::parseFile(const std::string& filename, bool validate)
{
	xmlParserCtxt* context = xmlCreateFileParserCtxt(filename.c_str());
	if (context == nullptr) {
		throw klay::Exception("Could not create parser context");
	}

	if (context->directory == nullptr) {
		context->directory = xmlParserGetDirectory(filename.c_str());
	}

	return parseContext(context, validate);
}

Document* Parser::parseString(const std::string& xml, bool validate)
{
	xmlParserCtxt* context = xmlCreateMemoryParserCtxt(xml.c_str(), xml.size() + 1);

	if (context == nullptr) {
		throw klay::Exception("Could not create parser context");
	}

	return parseContext(context, validate);
}

} // namespace xml
} // namespace klay
