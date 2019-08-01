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

#include <bluetooth.h>
#include <bluetooth-api.h>
#include <bluetooth_internal.h>

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

class BluetoothPolicy final : public AbstractPolicyProvider {
public:
	struct Bluetooth : public GlobalPolicy<DataSetInt> {
		Bluetooth();
		bool apply(const DataType&) override;
	};

	struct DesktopConnectivity : public GlobalPolicy<DataSetInt> {
		DesktopConnectivity();
		bool apply(const DataType&) override;
	};

	struct Paring : public GlobalPolicy<DataSetInt> {
		Paring();
		bool apply(const DataType&) override;
	};

	struct Tethering : public GlobalPolicy<DataSetInt> {
		Tethering();
		bool apply(const DataType&) override;
	};

	BluetoothPolicy();
	~BluetoothPolicy();

	BluetoothPolicy(const BluetoothPolicy&) = delete;
	BluetoothPolicy& operator=(const BluetoothPolicy&) = delete;

/* TODO: Support move semantic from parent class (GlobalPolicy)
	BluetoothPolicy(BluetoothPolicy&&) noexcept;
	BluetoothPolicy& operator=(BluetoothPolicy&&) noexcept;
*/
	void setBluetooth(bool enable);
	bool getBluetooth(void);

	void setDesktopConnectivity(bool enable);
	bool getDesktopConnectivity(void);

	void setParing(bool enable);
	bool getParing(void);

	void setTethering(bool enable);
	bool getTethering(void);

	static void onConnection(int result, bt_adapter_state_e state, void *user_data);

	static const std::string PRIVILEGE;

private:
	Bluetooth bluetooth;
	DesktopConnectivity desktopConnectivity;
	Paring Paring;
	Tethering tethering;

};

} // namespace osquery
