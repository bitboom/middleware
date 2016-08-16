/*
 * Tizen DPM Syspopup
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

#include <dpm/password.h>

#include "dpm-syspopup.h"

#define ARRAY_SIZE(_array_) \
	(sizeof(_array_) / sizeof(_array_[0]))

popup_info_s popup_list[] = {

	/* ID | TEXT_PREFIX | TITLE_TEXT | CONTENT_TEXT | POPUP_STYLE | LEFT_BUTTON | RIGHT_BUTTON | NOTI_TITLE | NOTI_CONTENT */

	/* Application Policy */
	{"package-installation-mode",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_INSTALLATION_OF_APPS",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"package-uninstallation-mode",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_UNINSTALLING_APPS",
		"toast",
		NULL, NULL, NULL, NULL
	},

	/* Password Policy */
	{"password-enforce-change",
		false,
		"IDS_ST_BODY_PASSWORD",
		"IDS_IDLE_POP_THE_SECURITY_POLICY_REQUIRES_YOU_TO_CHANGE_YOUR_PASSWORD",
		"default",
		NULL, "IDS_TPLATFORM_BUTTON_OK", NULL, NULL
	},

	/* Restriction Policy */
	{"wifi",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_WI_FI",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"wifi-hotspot",
		true,
		"IDS_MOBILEAP_MBODY_HOTSPOT_SPRINT",
		NULL,
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"camera",
		true,
		"IDS_IDLE_TAB4_CAMERA",
		NULL,
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"microphone",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_MIC",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"location",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_LOCATION_INFO",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"settings-changes",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_CHANGING_SETTINGS",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"bluetooth",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_BLUETOOTH",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"clipboard",
		true,
		"IDS_TPLATFORM_OPT_CLIPBOARD",
		NULL,
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"nfc",
		true,
		"IDS_ST_BODY_NFC",
		NULL,
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"message-sending",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_SENDING_MESSAGES",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"message-receiving",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_RETRIEVING_MESSAGES",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"browser",
		false,
		NULL,
		"IDS_IDLE_TPOP_SECURITY_POLICY_PREVENTS_USE_OF_BROWSER_APPS",
		"toast",
		NULL, NULL, NULL, NULL
	},
	{"screen-capture",
		true,
		"IDS_MF_BODY_SCREEN_CAPTURE_M_NOUN",
		NULL,
		"toast",
		NULL, NULL, NULL, NULL
	},

	/* Storage Policy */
	{"external-storage",   true, "IDS_DPM_EXTERNAL_STORAGE",   NULL, "toast", NULL, NULL, NULL, NULL},
	{"storage-decryption", true, "IDS_DPM_STORAGE_DECRYPTION", NULL, "toast", NULL, NULL, NULL, NULL},

	/* Krate Policy */
	{"krate-create",
		false,
		"IDS_DPM_KRATE_CREATE",
		"IDS_DPM_BODY_KRATE_CREATE",
		"default",
		"IDS_TPLATFORM_BUTTON2_CANCEL", "IDS_TPLATFORM_BUTTON_OK",
		"IDS_DPM_NOTI_KRATE_CREATE", "IDS_DPM_NOTI_BODY_KRATE_CREATE"
	},
	{"krate-remove",
		false,
		"IDS_DPM_KRATE_REMOVE",
		"IDS_DPM_BODY_KRATE_REMOVE",
		"default",
		"IDS_TPLATFORM_BUTTON2_CANCEL", "IDS_TPLATFORM_BUTTON_OK",
		"IDS_DPM_NOTI_KRATE_REMOVE", "IDS_DPM_NOTI_BODY_KRATE_REMOVE"
	},
};

popup_info_s *_get_popup_info(const char *id)
{
	int i = 0;

	if (id == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "popup_name is NULL");
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(popup_list); i++) {
		if (!strcmp(id, popup_list[i].id))
			return &popup_list[i];
	}

	dlog_print(DLOG_ERROR, LOG_TAG, "[%s] popup is not exist", id);
	return NULL;
}

int _get_popup_text(const char *id, const char *status, char *header, char *body)
{
	popup_info_s *info = NULL;
	char *lp_policy = NULL;
	char *lp_header = NULL;
	char *lp_body = NULL;

	info = _get_popup_info(id);
	if (info == NULL)
		return -1;

	if (info->title != NULL) {
		lp_policy = __(info->title);
		if (info->text_prefix) {
			lp_header = __("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
			snprintf(header, PATH_MAX, lp_header, lp_policy);
		} else {
			snprintf(header, PATH_MAX, "%s", lp_policy);
		}
	}

	if (info->content != NULL) {
		if (info->text_prefix) {
			lp_body = __("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
			snprintf(body, PATH_MAX, lp_body, lp_policy);
		} else {
			lp_body = __(info->content);
			snprintf(body, PATH_MAX, "%s", lp_body);
		}
	}

	return 0;
}
