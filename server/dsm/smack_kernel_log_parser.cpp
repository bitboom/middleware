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
 *  @file smack_kernel_log_parser.h
 *  @brief Implementation of parser for SMACK Kernel log.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#include <klay/audit/logger.h>

#include "smack_kernel_log_parser.h"

namespace DevicePolicyManager {

SmackKernelLogParser::SmackKernelLogParser(ISmackLogCollectorPtr logCollector):
	SmackLogParser(logCollector)
{
	try {
		m_data["caller"] = "Kernel";
	} catch (...) {
		ERROR("Caught exception");
	}
}

Json::Value SmackKernelLogParser::parseString(const std::string &stringToBeParsed)
{
	Json::Value builder(Json::ValueType::objectValue);

	std::vector<std::string> logNodesBeg{"subject=\"", "object=\"", "requested=", "action="};
	std::vector<std::string> logNodesEnd{"\"", "\"", " ", " "};
	std::vector<std::string> logNodeNames{"subject", "object", "access_type", "access_result"};

	std::string::size_type begPosition;
	std::string::size_type endPos;

	for (std::size_t i = 0; i < logNodesBeg.size(); ++i) {
		if ((begPosition = stringToBeParsed.find(logNodesBeg.at(i))) != std::string::npos &&
				(endPos = stringToBeParsed.find(logNodesEnd.at(i), begPosition + logNodesBeg[i].size())) != std::string::npos) {
			std::string nodeContent = stringToBeParsed.substr(logNodesBeg[i].size() + begPosition,
									  endPos - logNodesBeg[i].size() - begPosition);

			switch (i) {
			case 2: {
				if (nodeContent == "x") {
					builder[logNodeNames[i].c_str()] = "execute";
				} else if (nodeContent == "r") {
					builder[logNodeNames[i].c_str()] = "read";
				} else if (nodeContent == "w") {
					builder[logNodeNames[i].c_str()] = "write";
				}

				break;
			}
			default: {
				builder[logNodeNames[i].c_str()] = nodeContent;
				break;
			}
			}
		} else {
			ERROR("wrong log format");
			throw std::runtime_error("wrong log format");
		}
	}

	return builder;
}

int SmackKernelLogParser::calculateScore(const Json::Value &node)
{
	try {
		std::string currentAccessType(node["access_type"].asString());
		std::string currentAccessResult(node["access_result"].asString());

		int currentScore = 1;

		if (currentAccessType == "write") {
			currentScore *= 2;
		} else if (currentAccessType == "execute") {
			currentScore *= 3;
		}

		if (currentAccessResult == "denied") {
			currentScore *= 2;
		}

		return currentScore;
	} catch (...) {
		ERROR("caught exception due to wrong log format");

		return -1;
	}
}

} // namespace DevicePolicyManager
