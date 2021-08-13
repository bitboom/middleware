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

#include "krate-create.h"

KrateCreatePopup::KrateCreatePopup()
	: SyspopupInterface("IDS_DPM_KRATE_CREATE", "IDS_DPM_BODY_KRATE_CREATE"),
	  setupWizard("org.tizen.krate-setup-wizard")
{
}

KrateCreatePopup::~KrateCreatePopup()
{
}

void KrateCreatePopup::initialize()
{
	notification.setTitle("IDS_DPM_NOTI_KRATE_CREATE");
	notification.setContent("IDS_DPM_NOTI_BODY_KRATE_CREATE");
	notification.setImage("/usr/share/icons/default/small/org.tizen.dpm-syspopup.png");
	setupWizard.setData("mode", "create");
}

void KrateCreatePopup::contribute(const std::string &userData)
{
	setupWizard.setData("krate", userData);
	if (!isManufacturer("Tizen")) {
		setupWizard.setAppID("com.samsung.krate-setup-wizard");
	}
	notification.setAppControl(setupWizard);

	createDefaultPopup();
	if (popup == nullptr) {
		throw runtime::Exception("failed to create popup interface");
	}

	popup->onDispose = [this]() {
				notification.post();
				::ui_app_exit();
			};

	popup->setButton("IDS_TPLATFORM_BUTTON2_CANCEL",
			[this](void *event_info) {
				popup->dispose();
			},
			"IDS_TPLATFORM_BUTTON_OK",
			[this](void *event_info) {
				popup->onDispose = [this]() {
					setupWizard.launch();
					::ui_app_exit();
				};
				popup->dispose();
			});
}

void KrateCreatePopup::dispose()
{
}

SyspopupManager<KrateCreatePopup> krateCreatePopup("krate-create");
