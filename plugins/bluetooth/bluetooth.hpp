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

#include <vist/sdk/policy-model.hpp>
#include <vist/sdk/policy-provider.hpp>
#include <vist/sdk/policy-value.hpp>

namespace vist {
namespace policy {
namespace plugin {

class BluetoothState : public PolicyModel {
public:
	BluetoothState() : PolicyModel("bluetooth", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

class DesktopConnectivity : public PolicyModel {
public:
	DesktopConnectivity() : PolicyModel("bluetooth-desktop-connectivity", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

class Pairing: public PolicyModel {
public:
	Pairing() : PolicyModel("bluetooth-pairing", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

class Tethering: public PolicyModel {
public:
	Tethering() : PolicyModel("bluetooth-tethering", PolicyValue(1)) {}
	void onChanged(const PolicyValue&);
};

class Bluetooth final : public PolicyProvider {
public:
	Bluetooth(const std::string& name);
	~Bluetooth();
};

} // namespace plugin
} // namespace policy
} // namespace vist
