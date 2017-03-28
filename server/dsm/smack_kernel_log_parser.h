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
 *  @brief Parser for SMACK Kernel log.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#ifndef __DPM_SMACK_KERNEL_LOG_PARSER_H__
#define __DPM_SMACK_KERNEL_LOG_PARSER_H__

#include "smack_log_parser.h"

namespace DevicePolicyManager {

class SmackKernelLogParser : public SmackLogParser {
public:
	SmackKernelLogParser(ISmackLogCollectorPtr);

	Json::Value parseString(const std::string &stringToBeParsed) override;
	int calculateScore(const Json::Value &node) override;
};

} // namespace DevicePolicyManager

#endif //__DPM_SMACK_KERNEL_LOG_PARSER_H__
