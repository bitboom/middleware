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
 *  @file smack_kernel_log_collector.cpp
 *  @brief SMACK Kernel log collector.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 18, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#include <algorithm>

#include <klay/audit/logger.h>

#include "smack_kernel_log_collector.h"

namespace DevicePolicyManager {

const std::string PathToKernelLog("/var/log/dlog/kernel");

SmackKernelLogCollector::SmackKernelLogCollector():
	m_file(PathToKernelLog)
{
}

SmackKernelLogCollector::~SmackKernelLogCollector()
{
	m_file.close();
}


bool SmackKernelLogCollector::getLog(std::string &log)
{
	if (!m_file.is_open()) {
		return false;
	}

	return std::getline(m_file, log);
}

int SmackKernelLogCollector::getLogCount()
{
	int pos = m_file.tellg();

	int counter = std::count(std::istreambuf_iterator<char>(m_file),
							 std::istreambuf_iterator<char>(), '\n');

	m_file.seekg(pos);

	INFO("Count is : " + std::to_string(counter));

	return counter;
}

} // namespace DevicePolicyManager
