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
 *  @file smack.hxx
 *  @brief SMACK agent internal interface.
 *  @author Dmytro Logachev (d.logachev@samsung.com)
 *  @date Created Oct 13, 2016
 *  @par In Samsung Ukraine R&D Center (SRK) under a contract between
 *  @par LLC "Samsung Electronics Ukraine Company" (Kyiv, Ukraine)
 *  @par and "Samsung Electronics Co", Ltd (Seoul, Republic of Korea)
 *  @par Copyright: (c) Samsung Electronics Co, Ltd 2016. All rights reserved.
**/

#ifndef __SMACK_SECURITY_MONITOR__
#define __SMACK_SECURITY_MONITOR__

#include "policy-context.hxx"

namespace DevicePolicyManager {

/**
 * This class provides APIs to control smack security monitor functionalities
 */

class SmackSecurityMonitor {
public:
	SmackSecurityMonitor(PolicyControlContext &ctxt);
	~SmackSecurityMonitor();

	SmackSecurityMonitor(const SmackSecurityMonitor &rhs);
	SmackSecurityMonitor(SmackSecurityMonitor &&rhs);

	SmackSecurityMonitor &operator=(const SmackSecurityMonitor &rhs);
	SmackSecurityMonitor &operator=(SmackSecurityMonitor &&rhs);

	/**
	* @brief       Returns SMACK report.
	* @details     This API can be used for requesting current SMACK report
	* @return      String contains SMACK report
	*/
	std::string getReport();

	/**
	 * @brief       Returns number of SMACK issues.
	 * @details     This API can be used for requesting current number of of SMACK issues
	 * @return      Number of threats
	 */
	int getIssueCount();

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} // namespace DevicePolicyManager
#endif // __SMACK_SECURITY_MONITOR__
