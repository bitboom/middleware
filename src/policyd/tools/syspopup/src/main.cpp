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

#include <klay/exception.h>
#include "syspopup.h"

DPMSyspopup::DPMSyspopup()
	: Application("dpm-syspopup"), id(""), userData("")
{
}

DPMSyspopup::~DPMSyspopup()
{
}

void DPMSyspopup::onAppControl()
{
	SyspopupInterface *popup = nullptr;

	id = appControl->getData("id");
	if (ManagedPopup::list.find(id) == ManagedPopup::list.end()) {
		throw runtime::Exception("popup is not supported.");
	}

	popup = ManagedPopup::list[id];
	if (popup == nullptr) {
		throw runtime::Exception("failed to get popup instance");
	}

	popup->initialize();
	userData = appControl->getData("user-data");
	popup->contribute(userData);
	popup->show();
}

void DPMSyspopup::onTerminate()
{
	SyspopupInterface *popup = nullptr;
	popup = ManagedPopup::list[id];
	if (popup == nullptr) {
		throw runtime::Exception("failed to get popup instance");
	}
	popup->dispose();
}

int main(int argc, char *argv[])
{
	int ret = 0;
	DPMSyspopup syspopup;
	ret = syspopup.run(argc, argv);
	return ret;
}
