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

#include <wifi-manager.h>

namespace vist {
namespace policy {
namespace plugin {

struct WifiState : public PolicyModel {
	WifiState() : PolicyModel("wifi", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

struct ProfileChange : public PolicyModel {
	ProfileChange() : PolicyModel("wifi-profile-change", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

struct Hotspot : public PolicyModel {
	Hotspot() : PolicyModel("wifi-hotspot", PolicyValue(1)) {}
	void onChanged(const PolicyValue& value) override;
};

struct SsidRestriction : public PolicyModel {
	SsidRestriction() : PolicyModel("wifi-ssid-restriction", PolicyValue(0)) {}
	void onChanged(const PolicyValue& value) override;
};

class Wifi final : public PolicyProvider {
public:
	Wifi(const std::string& name);
	~Wifi();

private:
	::wifi_manager_h handle = nullptr;
};

} // namespace plugin
} // namespace policy
} // namespace vist
