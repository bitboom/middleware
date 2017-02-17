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

#include <vconf.h>

#include "dpm-syspopup.h"
#include "profile_decision.h"

static appdata_s password_enforce_change_appdata[] = {
	{"caller", "DPM"},
	{APP_CONTROL_DATA_SHIFT_WINDOW, "true"}
};

/* Exception case for platform */
static appdata_s password_verify_appdata[] = {
	{"caller", "DPM"},
	{APP_CONTROL_DATA_SHIFT_WINDOW, "true"},
	{"viewtype", "SETTING_PW_TYPE_ENTER_LOCK_TYPE"}
};

static appcontrol_s appcontrol_list[] = {
	{
		"password-enforce-change",
		"org.tizen.setting-locktype",
		password_enforce_change_appdata,
		ARRAY_SIZE(password_enforce_change_appdata)
	},
	/* Exception case for platform */
	{
		"password-verify",
		"org.tizen.setting-password",
		password_verify_appdata,
		ARRAY_SIZE(password_verify_appdata)
	}
};

static void reply_event_handler(app_control_h ug, app_control_h reply, app_control_result_e result, void *data)
{
	Evas_Object *popup = (Evas_Object *) data;
	char *result_string = NULL;
	char *current = NULL;

	if (result != APP_CONTROL_RESULT_SUCCEEDED) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to launch app");
		evas_object_del(popup);
		return;
	}

	app_control_get_extra_data(reply, "result", &result_string);
	if (result_string == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get reply.");
		evas_object_del(popup);
		return;
	}

	if (!strcmp(result_string, "SETTING_PW_TYPE_ENTER_LOCK_TYPE") ||
			!strcmp(result_string, "SETTING_PW_TYPE_VERIFY_FP_ALT_PASSWORD")) {
		free(result_string);
		app_control_get_extra_data(reply, "current", &current);
		if (current != NULL) {
			app_control_h app_control = NULL;
			app_control = (app_control_h)evas_object_data_get(popup, "app-control");
			if (app_control) {
				app_control_add_extra_data(app_control, "current", current);
				app_control_send_launch_request(app_control, NULL, NULL);
				app_control_destroy(app_control);
			}
			free(current);
		} else {
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current password");
		}
	}

	evas_object_del(popup);
	return;
}

static void confirm_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = (Evas_Object *) data;
	int lock_type = 0;
	app_control_h app_control = NULL;

	evas_object_data_set(popup, "status", "confirm");
	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);

	if (lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD
			|| lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
		app_control = create_syspopup_app_control(appcontrol_list, "password-verify");
		if (app_control) {
			app_control_send_launch_request(app_control, reply_event_handler, popup);
			app_control_destroy(app_control);
		}
	} else {
		app_control = (app_control_h)evas_object_data_get(popup, "app-control");
		if (app_control) {
			app_control_send_launch_request(app_control, NULL, NULL);
			app_control_destroy(app_control);
		}
		evas_object_del(popup);
	}
}

static void popup_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void create_popup_bottom_button(Evas_Object *popup, char *part, char *text, Evas_Smart_Cb callback)
{
	Evas_Object *button = NULL;

	if (_TIZEN_PROFILE_WEARABLE)
		button = create_button(popup, "bottom", __(text), callback, popup);
	else
		button = create_button(popup, "popup", __(text), callback, popup);

	elm_object_part_content_set(popup, part, button);
	return;
}

Evas_Object *create_password_enforce_change_popup(Evas_Object *parent, popup_info_s *info, void *user_data)
{
	Evas_Object *popup = NULL;
	Evas_Object *layout = NULL;
	app_control_h app_control = NULL;
	char header[PATH_MAX] = "";
	char body[PATH_MAX] = "";
	char *lp_text = NULL;

	if (_TIZEN_PROFILE_WEARABLE) {
		popup = create_popup(parent, "circle");
		elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

		layout = elm_layout_add(popup);
		elm_object_content_set(popup, layout);
		elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons1");
	}

	if (info->header != NULL && info->prefix) {
		lp_text = __("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
		snprintf(header, PATH_MAX, lp_text, __(info->header));
	} else if (info->header) {
		snprintf(header, PATH_MAX, "%s", __(info->header));
	}

	if (info->body != NULL && info->prefix) {
		lp_text = __("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
		snprintf(body, PATH_MAX, lp_text, __(info->body));
	} else if (info->body) {
		snprintf(body, PATH_MAX, "%s", __(info->body));
	}

	if (!_TIZEN_PROFILE_WEARABLE) {
		popup = create_popup(parent, "default");
		elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	}

	if (strcmp(header, "")) {
		if (_TIZEN_PROFILE_WEARABLE) {
			elm_object_part_text_set(layout, "elm.text.title", header);
		} else {
			elm_object_part_text_set(popup, "title,text", header);
			elm_object_item_part_text_translatable_set(popup, "title,text", EINA_TRUE);
		}
	}

	if (strcmp(body, "")) {
		if (_TIZEN_PROFILE_WEARABLE)
			elm_object_part_text_set(layout, "elm.text", body);
		else
			elm_object_text_set(popup, body);
	}

	create_popup_bottom_button(popup, "button1", info->right_btn, confirm_button_cb);
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, popup_deleted_cb, NULL);

	app_control = create_syspopup_app_control(appcontrol_list, info->id);
	evas_object_data_set(popup, "app-control", app_control);

	return popup;
}
