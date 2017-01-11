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

#include <system_info.h>

#include "dpm-syspopup.h"

static Eina_Bool key_event_cb(void *data, int type, void *event)
{
	Evas_Object *popup = (Evas_Object *)data;
	Evas_Event_Key_Down *ev = event;

	evas_object_data_set(popup, "status", "cancel");

	if (!strcmp(ev->keyname, "XF86Home") || !strcmp(ev->keyname, "XF86Back")) {
		evas_object_del(popup);
	}

	return EINA_TRUE;
}

void create_syspopup(const char *id, void *user_data, ui_data_s *ui_data)
{
	Evas_Object *window = NULL, *popup = NULL;
	popup_info_s *info = NULL;
	char *manufacturer = NULL;
	int ret = 0;

	info = get_popup_info(id);
	if (info == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup info");
		return;
	}

	window = create_window("dpm-syspopup");

	switch (info->viewtype) {
	case DPM_SYSPOPUP_DEFAULT:
		ret = system_info_get_platform_string("tizen.org/system/manufacturer", &manufacturer);
		if (ret != SYSTEM_INFO_ERROR_NONE) {
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to get system info");
			break;
		}

		if (!strcmp(manufacturer, "Tizen") && !strcmp(id, "password-enforce-change")) {
			popup = create_password_enforce_change_popup(window, info, NULL);
		} else {
			popup = create_default_popup(window, info, user_data);
			if (strcmp(id, "password-enforce-change")) {
				ui_data->key_event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, key_event_cb, popup);
			}
		}

		free(manufacturer);
		break;
	case DPM_SYSPOPUP_TOAST:
		popup = create_toast_popup(window, info, NULL);
		ui_data->key_event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, key_event_cb, popup);
		break;
	default:
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid view type");
		break;
	}

	if (popup)
		evas_object_show(popup);
	else
		ui_app_exit();

	return;
}
