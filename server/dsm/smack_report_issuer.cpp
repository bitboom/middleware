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
 *  @file smack_report_issuer.cpp
 *  @brief SMACK Report Issuer implementation.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#include <json/json.h>

#include <vector>
#include <string>

#include <klay/audit/logger.h>

#include "smack_report_issuer.h"
#include "smack_kernel_log_collector.h"
#include "smack_kernel_log_parser.h"

namespace DevicePolicyManager {

std::string SmackReportIssuer::generateReport()
{
	try {
		ISmackLogCollectorPtr kernelLogCollector = std::make_shared<SmackKernelLogCollector>();

		std::vector<SmackLogParserPtr> parsers;

		SmackLogParserPtr kernelParser = std::make_shared<SmackKernelLogParser>(kernelLogCollector);
		parsers.push_back(kernelParser);

		for (const auto &it : parsers)
			it->generateReport();

		Json::Value report(Json::ValueType::arrayValue);

		for (const auto &it : parsers)
			report.append(it->getRawReport());

		Json::FastWriter writer;

		return writer.write(report);
	} catch (...) {
		ERROR("Caugnt exception");

		return "[]";
	}
}

} // namespace DevicePolicyManager
