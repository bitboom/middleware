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

#pragma once

#include <string>
#include <stdexcept>

#include <wifi-manager.h>

/// TODO(Sangwan): Move rmi header to policy-model
#include <klay/rmi/service.h>
#include <dpm/pil/policy-model.h>

namespace osquery {

/*
	TODO List:
		1. Change VLOG to LOG(ERROR).
		2. Make sure that privilege model works with cynara.
		3. Consider policy-violation model.
		4. Unify exeception handling among getter/setter APIs.
		5. Verify with full-DPM at runtime.
*/

class WifiPolicy final : public AbstractPolicyProvider {
public:
	struct Wifi : public GlobalPolicy<DataSetInt> {
		Wifi();
		bool apply(const DataType&) override;
	};

	struct Profile : public GlobalPolicy<DataSetInt> {
		Profile();
		bool apply(const DataType&) override;
	};

	struct Hotspot : public GlobalPolicy<DataSetInt> {
		Hotspot();
		bool apply(const DataType&) override;
	};

	WifiPolicy();
	~WifiPolicy();

	WifiPolicy(const WifiPolicy&) = delete;
	WifiPolicy& operator=(const WifiPolicy&) = delete;

/* TODO: Support move semantic from parent class (GlobalPolicy)
	WifiPolicy(WifiPolicy&&) noexcept;
	WifiPolicy& operator=(WifiPolicy&&) noexcept;
*/
	void setWifi(bool enable);
	bool getWifi(void);

	void setProfile(bool enable);
	bool getProfile(void);

	void setHotspot(bool enable);
	bool getHotspot(void);

	static void onConnection(wifi_manager_connection_state_e state,
							 wifi_manager_ap_h ap,
							 void *user_data);

	static const std::string PRIVILEGE;

private:
	Wifi wifi;
	Profile profile;
	Hotspot hotspot;

	wifi_manager_h handle;
};

} // namespace osquery
