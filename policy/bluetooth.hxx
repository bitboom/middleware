/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __BLUETOOTH_POLICY__
#define __BLUETOOTH_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

/**
 * This class provides APIs to control bluetooth functionalities
 * such as bluetooth device or uuid restriction
 */

class BluetoothPolicy {
public:
	BluetoothPolicy(PolicyControlContext& ctxt);
	~BluetoothPolicy();

	// for restriction CPIs
	int setModeChangeState(bool enable);
	bool getModeChangeState();
	int setDesktopConnectivityState(bool enable);
	bool getDesktopConnectivityState();
	int setTetheringState(bool enable);
	bool getTetheringState();
	int setPairingState(bool enable);
	bool getPairingState();

	// for bluetooth CAPIs
	int addDeviceToBlacklist(const std::string& mac);
	int removeDeviceFromBlacklist(const std::string& mac);
	int setDeviceRestriction(bool enable);
	bool isDeviceRestricted();

	int addUuidToBlacklist(const std::string& uuid);
	int removeUuidFromBlacklist(const std::string& uuid);
	int setUuidRestriction(bool enable);
	bool isUuidRestricted();

private:
	PolicyControlContext& context;
};

} // namespace DevicePolicyManager
#endif // __BLUETOOTH_POLICY__
