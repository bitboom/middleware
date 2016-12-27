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

#define _TIZEN_PROFILE_WEARABLE (0)

#include <system_info.h>

#include "dpm-syspopup.h"

static const char icon_path[] = "/usr/share/icons/default/small/org.tizen.dpm-syspopup.png";
static const char *delete_icon_file = "/usr/apps/org.tizen.dpm-syspopup/res/images/tw_ic_popup_btn_delete.png";
static const char *check_icon_file = "/usr/apps/org.tizen.dpm-syspopup/res/images/tw_ic_popup_btn_check.png";

static appdata_s krate_create_appdata[] = {
	{"mode", "create"}
};

static appdata_s krate_remove_appdata[] = {
	{"mode", "remove"}
};

static appdata_s password_enforce_change_appdata[] = {
	{"caller", "dpm"}
};

static appcontrol_s z3_appcontrol_list[] = {
	{
		"krate-create",
		"org.tizen.krate-setup-wizard",
		krate_create_appdata,
		ARRAY_SIZE(krate_create_appdata)
	},
	{
		"krate-remove",
		"org.tizen.krate-setup-wizard",
		krate_remove_appdata,
		ARRAY_SIZE(krate_remove_appdata)
	}
};

static appcontrol_s z2_appcontrol_list[] = {
	{
		"krate-create",
		"com.samsung.krate-setup-wizard",
		krate_create_appdata,
		ARRAY_SIZE(krate_create_appdata)
	},
	{
		"krate-remove",
		"com.samsung.krate-setup-wizard",
		krate_remove_appdata,
		ARRAY_SIZE(krate_remove_appdata)
	},
	{
		"password-enforce-change",
		"com.samsung.setting.locktype",
		password_enforce_change_appdata,
		ARRAY_SIZE(password_enforce_change_appdata)
	}
};

static notification_s notification_list[] = {
	{
		"krate-create",
		"IDS_DPM_NOTI_KRATE_CREATE",
		"IDS_DPM_NOTI_BODY_KRATE_CREATE",
		icon_path
	},
	{
		"krate-remove",
		"IDS_DPM_NOTI_KRATE_REMOVE",
		"IDS_DPM_NOTI_BODY_KRATE_REMOVE",
		icon_path
	}
};

static void cancel_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = (Evas_Object *) data;
	evas_object_data_set(popup, "status", "cancel");
	evas_object_del(popup);
	return;
}

static void confirm_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = (Evas_Object *) data;
	app_control_h app_control = NULL;
	int ret = 0;

	/* call application */
	app_control = (app_control_h)evas_object_data_get(popup, "app-control");
	if (app_control) {
		ret = app_control_send_launch_request(app_control, NULL, NULL);
		if (ret != APP_CONTROL_ERROR_NONE)
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to send launch request");
	}

	evas_object_data_set(popup, "status", "confirm");
	evas_object_del(popup);
	return;
}

static void popup_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	int ret = 0;
	char *status = NULL;

	status = (char *)evas_object_data_get(obj, "status");

	if (status != NULL && !strcmp(status, "cancel")) {
		popup_info_s *info = (popup_info_s *)data;
		app_control_h app_control = NULL;
		app_control = (app_control_h)evas_object_data_get(obj, "app-control");

		/* create notification */
		ret = create_syspopup_notification(notification_list, info->id, app_control);
		if (ret != 0) {
			dlog_print(DLOG_ERROR, LOG_TAG, "cannot create notification");
		}
		if (app_control) {
			app_control_destroy(app_control);
		}
	}
	ui_app_exit();
}

static void create_popup_bottom_button(Evas_Object *popup, const char *part, char *text, Evas_Smart_Cb callback)
{
	Evas_Object *button = NULL;

	if (_TIZEN_PROFILE_WEARABLE)
		button = create_button(popup, "bottom", __(text), callback, popup);
	else
		button = create_button(popup, "popup", __(text), callback, popup);
	elm_object_part_content_set(popup, part, button);

	return;
}

static void create_popup_left_button(Evas_Object *popup, Evas_Smart_Cb callback)
{
	Evas_Object *button = NULL;
	Evas_Object *icon = NULL;

	button = create_button(popup, "popup/circle/left", NULL, callback, popup);
	icon = create_image(button, delete_icon_file);
	elm_object_part_content_set(button, "elm.swallow.content", icon);
	elm_object_part_content_set(popup, "button1", button);
	return;
}

static void create_popup_right_button(Evas_Object *popup, Evas_Smart_Cb callback)
{
	Evas_Object *button = NULL;
	Evas_Object *icon = NULL;

	button = create_button(popup, "popup/circle/right", NULL, callback, popup);
	icon = create_image(button, check_icon_file);
	elm_object_part_content_set(button, "elm.swallow.content", icon);

	elm_object_part_content_set(popup, "button2", button);
	return;
}

static void set_appcontrol(Evas_Object *popup, const char *id, void *user_data)
{
	app_control_h app_control = NULL;
	char *manufacturer = NULL;

	system_info_get_platform_string("tizen.org/system/manufacturer", &manufacturer);

	if (!strcmp(manufacturer, "Tizen")) {
		app_control = create_syspopup_app_control(z3_appcontrol_list, id);
	} else if (!strcmp(manufacturer, "Samsung")) {
		app_control = create_syspopup_app_control(z2_appcontrol_list, id);
	} else {
		free(manufacturer);
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid manufacturer");
		return;
	}

	free(manufacturer);

	if (app_control) {
		if (!strcmp(id, "krate-create") || !strcmp(id, "krate-remove")) {
			app_control_add_extra_data(app_control, "krate", (char *)user_data);
		}
		evas_object_data_set(popup, "app-control", app_control);
	}
	return;
}

Evas_Object *create_default_popup(Evas_Object *parent, popup_info_s *info, void *user_data)
{
	Evas_Object *popup = NULL;
	Evas_Object *layout = NULL;
	char header[PATH_MAX] = "";
	char body[PATH_MAX] = "";
	char *lp_text = NULL;

	if (_TIZEN_PROFILE_WEARABLE)
		popup = create_popup(parent, "circle");
	else
		popup = create_popup(parent, "default");

	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

	if (_TIZEN_PROFILE_WEARABLE) {
		layout = elm_layout_add(popup);
		elm_object_content_set(popup, layout);

		if (info->left_btn != NULL) {
			elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
			create_popup_left_button(popup, cancel_button_cb);
			create_popup_right_button(popup, confirm_button_cb);
		} else {
			elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons1");
			create_popup_bottom_button(popup, "button1", __(info->right_btn), confirm_button_cb);
		}
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

	if (!_TIZEN_PROFILE_WEARABLE) {
		if (info->left_btn) {
			create_popup_bottom_button(popup, "button1", info->left_btn, cancel_button_cb);
			create_popup_bottom_button(popup, "button2", info->right_btn, confirm_button_cb);
		} else {
			create_popup_bottom_button(popup, "button1", info->right_btn, confirm_button_cb);
		}
	}

	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, popup_deleted_cb, info);
	set_appcontrol(popup, info->id, user_data);

	return popup;
}
