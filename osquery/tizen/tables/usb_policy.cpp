/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file usb_policy.cpp
 * @author Sangwan Kwon (sangwan.kwon@samsung.com)
 * @brief Implementation of usb_policy table
 */

#include <string>
#include <memory>
#include <stdexcept>

#include <osquery/sql.h>
#include <osquery/logger.h>
#include <osquery/tables.h>

#include <dpm/device-policy-manager.h>
#include <dpm/pil/policy-client.h>

namespace osquery {
namespace tables {

QueryData genUsbPolicy(QueryContext& context) try {
	std::shared_ptr<void> handle(dpm_manager_create(), dpm_manager_destroy);
	if (handle == nullptr)
		throw std::runtime_error("Cannot create dpm-client handle.");

	/// This status is defined at DPM
	::Status<bool> status { true };
	Row r;

	DevicePolicyClient &client = GetDevicePolicyClient(handle.get());
	status = client.methodCall<bool>("Usb::getDebuggingState");
	r["usb_debugging"] =  INTEGER(status.get());

	status = client.methodCall<bool>("Usb::getTetheringState");
	r["usb_tethering"] =  INTEGER(status.get());

	status = client.methodCall<bool>("Usb::getClientState");
	r["usb_client"] =  INTEGER(status.get());

	return { r };
} catch (...) {
// TODO(Sangwan): Resolve duplicated "ERROR" macro with DPM
	Row r;
	return { r };
}

} // namespace tables
} // namespace osquery
