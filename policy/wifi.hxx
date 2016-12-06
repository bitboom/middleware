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

#ifndef __WIFI_POLICY__
#define __WIFI_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

/**
 * This class provides APIs to configure Wi-Fi related settings and manage Wi-Fi profiles.
 */

class WifiPolicy {
public:
	WifiPolicy(PolicyControlContext& ctxt);
	~WifiPolicy(void);

	int setState(bool enable);
	bool getState();

	int setHotspotState(bool enable);
	bool getHotspotState();

	int setProfileChangeRestriction(bool restrict);
	bool isProfileChangeRestricted();

	int setNetworkAccessRestriction(bool restrict);
	bool isNetworkAccessRestricted();

	int addSsidToBlocklist(const std::string& ssid);
	int removeSsidFromBlocklist(const std::string& ssid);

private:
	PolicyControlContext& context;
};

} // namespace DevicePolicyManager
#endif // __WIFI_POLICY__
