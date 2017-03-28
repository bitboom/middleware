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
 *  @file smack_log_parser.h
 *  @brief Interface for SMACK log parsers.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#ifndef __DPM_SMACK_LOG_PARSER_H__
#define __DPM_SMACK_LOG_PARSER_H__

#include <memory>
#include <string>
#include <json/json.h>

#include "i_smack_log_collector.h"

namespace DevicePolicyManager {

class SmackLogParser {
public:
	virtual ~SmackLogParser() = default;

	SmackLogParser(ISmackLogCollectorPtr);

	virtual Json::Value &getRawReport();

	virtual Json::Value parseString(const std::string &stringToBeParsed) = 0;

	virtual bool generateAnalytics(const Json::Value &node);

	virtual int calculateScore(const Json::Value &node) = 0;

	virtual bool append(const Json::Value &node);

	virtual std::string getParsedReport();

	virtual int getIssueCount();

	virtual bool generateReport();

protected:
	Json::Value m_data;
	int m_issueCount;

private:
	ISmackLogCollectorPtr m_logCollector;
};

using SmackLogParserPtr = std::shared_ptr<SmackLogParser>;

} // namespace DevicePolicyManager

#endif //__DPM_SMACK_LOG_PARSER_H__
