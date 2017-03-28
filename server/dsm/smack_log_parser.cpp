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

/**
 *  @file smack_log_parser.cpp
 *  @brief SMACK log parser interface implementation.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#include <string>
#include <algorithm>

#include <klay/audit/logger.h>

#include "smack_log_parser.h"

namespace DevicePolicyManager {

static const int MAX_SMACK_LOG = 50;

SmackLogParser::SmackLogParser(ISmackLogCollectorPtr logCollector):
	m_data(Json::ValueType::objectValue),
	m_issueCount(0),
	m_logCollector(logCollector)
{
	try {
		/*Expected template here: {\"arr\":[]} */
		m_data["arr"] = Json::Value(Json::ValueType::arrayValue);
		m_data["analytics"] = Json::Value(Json::ValueType::arrayValue);
	} catch (...) {
		ERROR("caught exception");
	}
}

Json::Value &SmackLogParser::getRawReport()
{
	return m_data;
}

bool SmackLogParser::append(const Json::Value &node)
{
	try {
		m_data["arr"].append(node);
		m_issueCount++;

		return true;
	} catch (...) {
		ERROR("Couldn't append");

		return false;
	}
}

std::string SmackLogParser::getParsedReport()
{
	Json::FastWriter fastWriter;

	return fastWriter.write(m_data);
}

int SmackLogParser::getIssueCount()
{
	return m_issueCount;
}

bool SmackLogParser::generateReport()
{

	std::string currentString;

	int logCount = m_logCollector->getLogCount();
	int absoluteCount = 0;

	while (m_logCollector->getLog(currentString)) {
		try {
			Json::Value value(parseString(currentString));

			if (logCount - absoluteCount <= MAX_SMACK_LOG) {
				append(value);
			} else {
				m_issueCount++;
			}

			generateAnalytics(value);
		} catch (...) {
			ERROR("Caught exception");

			continue;
		}

		absoluteCount++;
	}

	try {
		m_data["actual_count"] = m_issueCount;
	} catch (...) {
		ERROR("caught exception");
	}

	return true;
}

bool SmackLogParser::generateAnalytics(const Json::Value &node)
{
	try {
		std::string currentObject(node["object"].asString());
		std::string currentSubject(node["subject"].asString());

		int currentScore = calculateScore(node);

		if (currentScore == -1) {
			ERROR("failed to calculate score");

			return false;
		}

		Json::Value::iterator objectIterator = std::find_if(m_data["analytics"].begin(),
											   m_data["analytics"].end(),
		[&currentObject](const Json::Value & val) {
			return currentObject == val["object"].asString();
		});

		bool ifObjectExists = false;

		if (objectIterator != m_data["analytics"].end()) {
			ifObjectExists = true;
		}

		if (!ifObjectExists) {
			Json::Value nodeToBeAppended(Json::ValueType::objectValue);
			nodeToBeAppended["object"] = currentObject;
			nodeToBeAppended["score"] = currentScore;
			nodeToBeAppended["cnt"] = 1;
			nodeToBeAppended["subjects"] = Json::Value(Json::ValueType::arrayValue);
			Json::Value subNode(Json::ValueType::objectValue);
			subNode["name"] = currentSubject;
			subNode["score"] = currentScore;
			subNode["cnt"] = 1;
			nodeToBeAppended["subjects"].append(subNode);
			m_data["analytics"].append(nodeToBeAppended);

			return true;
		} else {
			(*objectIterator)["score"] = (*objectIterator)["score"].asInt() + currentScore;
			(*objectIterator)["cnt"] = (*objectIterator)["cnt"].asInt() + 1;
		}

		Json::Value::iterator subjectIteraror = std::find_if((*objectIterator)["subjects"].begin(),
												(*objectIterator)["subjects"].end(),
		[&currentSubject](const Json::Value & val) {
			return currentSubject == val["name"].asString();
		});

		bool ifSubjectExists = false;

		if (subjectIteraror != (*objectIterator)["subjects"].end()) {
			ifSubjectExists = true;
		}

		if (ifSubjectExists) {
			(*subjectIteraror)["score"] = (*subjectIteraror)["score"].asInt() + currentScore;
			(*subjectIteraror)["cnt"] = (*subjectIteraror)["cnt"].asInt() + 1;
		} else {
			Json::Value subNode(Json::ValueType::objectValue);
			subNode["name"] = currentSubject;
			subNode["score"] = currentScore;
			subNode["cnt"] = 1;
			(*objectIterator)["subjects"].append(subNode);
		}

		return true;
	} catch (...) {
		ERROR("caught exception");

		return false;
	}
}

} // namespace DevicePolicyManager
