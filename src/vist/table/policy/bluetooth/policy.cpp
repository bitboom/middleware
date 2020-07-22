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

#include "policy.hpp"

#include <vist/logger.hpp>
#include <vist/exception.hpp>

#include <bluetooth.h>
#include <bluetooth-api.h>
#include <bluetooth_internal.h>

namespace vist {
namespace policy {

namespace {

bool failed(int value)
{
	return value == BLUETOOTH_DPM_RESULT_ACCESS_DENIED || value == BLUETOOTH_DPM_RESULT_FAIL;
}

auto allowed(int value)
{
	return value ? BLUETOOTH_DPM_ALLOWED : BLUETOOTH_DPM_RESTRICTED;
}

auto bt_allowed(int value)
{
	return value ? BLUETOOTH_DPM_BT_ALLOWED : BLUETOOTH_DPM_BT_RESTRICTED;
}

} // anonymous namespace

void BluetoothState::onChanged(const PolicyValue& value)
{
	auto enable = bt_allowed(value);
	auto ret = ::bluetooth_dpm_set_allow_mode(enable);
	if (failed(ret))
		THROW(ErrCode::RuntimeError) << "Failed to change bluetooth state: " << ret;

	INFO(VIST_PLUGIN) << "Bluetooth state is changed to " << enable;
}

void DesktopConnectivity::onChanged(const PolicyValue& value)
{
	auto enable = allowed(value);
	auto ret = ::bluetooth_dpm_set_desktop_connectivity_state(enable);
	if (failed(ret))
		THROW(ErrCode::RuntimeError) << "Failed to change bt_desktop_connectivity: " << ret;

	INFO(VIST_PLUGIN) << "Bluetooth desktop connectivity state is changed to " << enable;
}

void Pairing::onChanged(const PolicyValue& value)
{
	auto enable = allowed(value);
	auto ret = ::bluetooth_dpm_set_pairing_state(enable);
	if (failed(ret))
		THROW(ErrCode::RuntimeError) << "Failed to change bluetooth pairing: " << ret;

	INFO(VIST_PLUGIN) << "Bluetooth pairing state is changed to " << enable;
}

void Tethering::onChanged(const PolicyValue& value)
{
	auto enable = value;
	INFO(VIST_PLUGIN) << "Bluetooth tethering state is changed to " << enable;
}

Bluetooth::Bluetooth(const std::string& name) : PolicyProvider(name)
{
	if (::bt_initialize() != BT_ERROR_NONE)
		THROW(ErrCode::RuntimeError) << "Failed to init bluetooth provider.";
}

Bluetooth::~Bluetooth()
{
	::bt_deinitialize();
}

} // namespace policy
} // namespace vist
