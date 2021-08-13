/*
 *  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include "schema.hpp"

namespace vist {
namespace policy {

class BluetoothProvider final : public PolicyProvider {
public:
	BluetoothProvider(const std::string& name);
	~BluetoothProvider();

	struct State : public PolicyModel {
		State();
		void onChanged(const PolicyValue& value) override;
	};

	struct DesktopConnectivity : public PolicyModel {
		DesktopConnectivity();
		void onChanged(const PolicyValue& value) override;
	};

	struct Pairing: public PolicyModel {
		Pairing();
		void onChanged(const PolicyValue& value) override;
	};

	struct Tethering: public PolicyModel {
		Tethering();
		void onChanged(const PolicyValue&) override;
	};

};

template <typename Column>
std::string GetPolicyName(const Column& column) {
	return schema::bluetooth.name + "-" + column.name;
}

} // namespace policy
} // namespace vist
