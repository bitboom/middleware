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

#include <vconf.h>
#include "password.h"

PasswordPopup::PasswordPopup()
	: SyspopupInterface("IDS_ST_BODY_PASSWORD", "IDS_IDLE_POP_THE_SECURITY_POLICY_REQUIRES_YOU_TO_CHANGE_YOUR_PASSWORD"),
	  setAppControl("org.tizen.setting-locktype"), verifyAppControl("org.tizen.setting-password")
{
}

PasswordPopup::~PasswordPopup()
{
}

void PasswordPopup::initialize()
{
	setAppControl.setData("caller", "DPM");
	setAppControl.setData(APP_CONTROL_DATA_SHIFT_WINDOW, "true");
}

void PasswordPopup::contribute(const std::string &userData)
{
	createDefaultPopup();
	if (popup == nullptr) {
		throw runtime::Exception("Failed to create popup interface");
	}

	popup->deleteEcoreHandler(ECORE_EVENT_KEY_DOWN);

	auto replyHandler = [this](AppControl *ug, AppControl *reply, int result) {
		if (result != APP_CONTROL_RESULT_SUCCEEDED) {
			popup->dispose();
			return;
		}
		std::string resultData = reply->getData("result");

		if (resultData.compare("SETTING_PW_TYPE_ENTER_LOCK_TYPE") == 0
						|| resultData.compare("SETTING_PW_TYPE_VERIFY_FP_ALT_PASSWORD") == 0) {
			std::string current(reply->getData("current"));
			if (!current.empty()) {
				setAppControl.setData("current", current);
				setAppControl.launch();
			}
		}
		popup->dispose();
	};

	auto z3ConfirmEvent = [this, replyHandler](void *event_info) {
		int lockType = 0;
		::vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lockType);

		if (lockType == SETTING_SCREEN_LOCK_TYPE_PASSWORD
				|| lockType == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
			verifyAppControl.setData("caller", "DPM");
			verifyAppControl.setData(APP_CONTROL_DATA_SHIFT_WINDOW, "true");
			verifyAppControl.setData("viewtype", "SETTING_PW_TYPE_ENTER_LOCK_TYPE");
			verifyAppControl.launch(std::forward<AppControl::ReplyHandler>(replyHandler));
		} else {
			setAppControl.launch();
			popup->dispose();
		}
	};

	auto z2ConfirmEvent = [this](void *event_info) {
		AppControl setPassword("com.samsung.setting.locktype");
		setPassword.setData("caller", "dpm");
		setPassword.launch();
	};

	if (isManufacturer("Tizen")) {
		popup->setButton("IDS_TPLATFORM_BUTTON_OK", std::forward<Widget::SmartHandler>(z3ConfirmEvent));
	} else {
		popup->setButton("IDS_TPLATFORM_BUTTON_OK", std::forward<Widget::SmartHandler>(z2ConfirmEvent));
	}
}

void PasswordPopup::dispose()
{
}

SyspopupManager<PasswordPopup> passwordPopup("password-enforce-change");
