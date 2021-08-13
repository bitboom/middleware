/*
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "bluetooth.h"

BluetoothPopup::BluetoothPopup()
	: SyspopupInterface("IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_BLUETOOTH")
{
}

BluetoothPopup::~BluetoothPopup()
{
}

void BluetoothPopup::initialize()
{
}

void BluetoothPopup::contribute(const std::string &userData)
{
	createToastPopup();
}

void BluetoothPopup::dispose()
{
}

SyspopupManager<BluetoothPopup> bluetoothPopup("bluetooth");
