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

class UsbPolicy final : public AbstractPolicyProvider {
public:
	/// usb-client
	struct Usb : public GlobalPolicy<DataSetInt> {
		Usb();
		bool apply(const DataType&) override;
	};

	struct Debugging : public GlobalPolicy<DataSetInt> {
		Debugging();
		bool apply(const DataType&) override;
	};

	struct Tethering : public GlobalPolicy<DataSetInt> {
		Tethering();
		bool apply(const DataType&) override;
	};

	UsbPolicy() = default;
	~UsbPolicy() = default;

	UsbPolicy(const UsbPolicy&) = delete;
	UsbPolicy& operator=(const UsbPolicy&) = delete;

/* TODO: Support move semantic from parent class (GlobalPolicy)
	UsbPolicy(UsbPolicy&&) noexcept;
	UsbPolicy& operator=(UsbPolicy&&) noexcept;
*/
	void setUsb(bool enable);
	bool getUsb(void);

	void setDebugging(bool enable);
	bool getDebugging(void);

	void setTethering(bool enable);
	bool getTethering(void);

	static const std::string PRIVILEGE;

private:
	Usb Usb;
	Debugging debugging;
	Tethering tethering;
};

} // namespace osquery
