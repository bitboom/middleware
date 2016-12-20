/*
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "dpm-syspopup.h"

popup_info_s popup_list[] = {
	/* ID | VIEW_TYPE | TEXT_PREFIX | TITLE_TEXT | CONTENT_TEXT | LEFT_BUTTON | RIGHT_BUTTON */

	/* Application Policy */
	{
		"package-installation-mode",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_INSTALLATION_OF_APPS",
		NULL, NULL
	},
	{
		"package-uninstallation-mode",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_UNINSTALLING_APPS",
		NULL, NULL
	},

	/* Password Policy */
	{
		"password-enforce-change",
		DPM_SYSPOPUP_DEFAULT,
		false,
		"IDS_ST_BODY_PASSWORD",
		"IDS_IDLE_POP_THE_SECURITY_POLICY_REQUIRES_YOU_TO_CHANGE_YOUR_PASSWORD",
		NULL, "IDS_TPLATFORM_BUTTON_OK"
	},

	/* Restriction Policy */
	{
		"wifi",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_WI_FI",
		NULL, NULL
	},
	{
		"wifi-hotspot",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_MOBILEAP_MBODY_HOTSPOT_SPRINT",
		"IDS_MOBILEAP_MBODY_HOTSPOT_SPRINT",
		NULL, NULL
	},
	{
		"camera",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_IDLE_TAB4_CAMERA",
		"IDS_IDLE_TAB4_CAMERA",
		NULL, NULL
	},
	{
		"microphone",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_MIC",
		NULL, NULL
	},
	{
		"location",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_LOCATION_INFO",
		NULL, NULL
	},
	{
		"settings-changes",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_CHANGING_SETTINGS",
		NULL, NULL
	},
	{
		"bluetooth",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_BLUETOOTH",
		NULL, NULL
	},
	{
		"clipboard",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_TPLATFORM_OPT_CLIPBOARD",
		"IDS_TPLATFORM_OPT_CLIPBOARD",
		NULL, NULL
	},
	{
		"nfc",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_ST_BODY_NFC",
		"IDS_ST_BODY_NFC",
		NULL, NULL
	},
	{
		"message-sending",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_SENDING_MESSAGES",
		NULL, NULL
	},
	{
		"message-retrieving",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_RETRIEVING_MESSAGES",
		NULL, NULL
	},
	{
		"browser",
		DPM_SYSPOPUP_TOAST,
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_BROWSER_APPS",
		NULL, NULL
	},
	{
		"screen-capture",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_MF_BODY_SCREEN_CAPTURE_M_NOUN",
		"IDS_MF_BODY_SCREEN_CAPTURE_M_NOUN",
		NULL, NULL
	},

	/* Storage Policy */
	{
		"external-storage",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_DPM_EXTERNAL_STORAGE",
		NULL,
		NULL, NULL
	},
	{
		"storage-decryption",
		DPM_SYSPOPUP_TOAST,
		true,
		"IDS_DPM_STORAGE_DECRYPTION",
		NULL,
		NULL, NULL
	},

	/* Krate Policy */
	{
		"krate-create",
		DPM_SYSPOPUP_DEFAULT,
		false,
		"IDS_DPM_KRATE_CREATE",
		"IDS_DPM_BODY_KRATE_CREATE",
		"IDS_TPLATFORM_BUTTON2_CANCEL", "IDS_TPLATFORM_BUTTON_OK"
	},
	{
		"krate-remove",
		DPM_SYSPOPUP_DEFAULT,
		false,
		"IDS_DPM_KRATE_REMOVE",
		"IDS_DPM_BODY_KRATE_REMOVE",
		"IDS_TPLATFORM_BUTTON2_CANCEL", "IDS_TPLATFORM_BUTTON_OK"
	},
};

popup_info_s *get_popup_info(const char *id)
{
	int i = 0;

	if (id == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "popup id is NULL");
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(popup_list); i++) {
		if (!strcmp(id, popup_list[i].id)) {
			return &popup_list[i];
		}
	}

	dlog_print(DLOG_ERROR, LOG_TAG, "[%s] popup is not exist", id);
	return NULL;
}
