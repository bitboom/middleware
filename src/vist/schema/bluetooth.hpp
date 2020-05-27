/*
 *  Copyright (c) 2020-present Samsung Electronics Co., Ltd All Rights Reserved
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

#include <vist/query-builder.hpp>

namespace vist {
namespace schema {

struct Bluetooth {
	int state;
	int desktopConnectivity;
	int pairing;
	int tethering;

	DECLARE_COLUMN(State, "state", &Bluetooth::state);
	DECLARE_COLUMN(DesktopConnectivity, "desktopConnectivity", &Bluetooth::desktopConnectivity);
	DECLARE_COLUMN(Pairing, "pairing", &Bluetooth::pairing);
	DECLARE_COLUMN(Tethering, "tethering", &Bluetooth::tethering);
};

DECLARE_TABLE(BluetoothTable, "bluetooth", Bluetooth::State,
			  Bluetooth::DesktopConnectivity,
			  Bluetooth::Pairing,
			  Bluetooth::Tethering);

} // namesapce schema
} // namesapce vist
