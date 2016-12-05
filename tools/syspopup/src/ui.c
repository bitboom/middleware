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

#include <notification.h>
#include <app_control_internal.h>
#include <efl_util.h>
#include <vconf.h>

#include "dpm-syspopup.h"

static Evas_Object *main_window;

static void __win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void create_window(const char *pkg_name)
{
	int rots[] = {0, 90, 180, 270};
	main_window = elm_win_add(NULL, pkg_name, ELM_WIN_NOTIFICATION);
	efl_util_set_notification_window_level(main_window, EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT);

	if (elm_win_wm_rotation_supported_get(main_window))
		elm_win_wm_rotation_available_rotations_set(main_window, rots, 4);

	elm_win_title_set(main_window, pkg_name);
	elm_win_borderless_set(main_window, EINA_TRUE);
	elm_win_alpha_set(main_window, EINA_TRUE);

	evas_object_smart_callback_add(main_window, "delete,request", __win_delete_request_cb, NULL);
	evas_object_show(main_window);
}

static void __popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void __popup_timeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
	return;
}

static void __block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
	return;
}

static void default_ok_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = (Evas_Object *) data;
	evas_object_data_set(popup, "selected", "ok");

	/* call application */
	app_control_h app_control = (app_control_h)evas_object_data_get(popup, "app-control");

	if (app_control_send_launch_request(app_control, NULL, NULL) != APP_CONTROL_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to send launch request");

	app_control_destroy(app_control);
	evas_object_del(popup);
	return;
}

static void reply_password_enforce_event_handler(app_control_h ug, app_control_h reply, app_control_result_e result, void* data)
{
	Evas_Object *popup = (Evas_Object *) data;
	if (!result) {
		char *result_string = NULL;
		char *current = NULL;

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
				app_control_h app_control;
				if (app_control_create(&app_control) != APP_CONTROL_ERROR_NONE) {
					dlog_print(DLOG_ERROR, LOG_TAG, "faild to create app_control context");
					evas_object_del(popup);
					return;
				}

				app_control_add_extra_data(app_control, "current", current);
				app_control_add_extra_data(app_control, "caller", "DPM");
				app_control_set_app_id(app_control, "org.tizen.setting-locktype");
				app_control_send_launch_request(app_control, NULL, NULL);
				app_control_destroy(app_control);
				free(current);
			} else {
				dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current password.");
				evas_object_del(popup);
				return;
			}
		}
	}
	evas_object_del(popup);
}

static void password_enforce_event_handler(void *data, Evas_Object *obj, void *event_info)
{
	int lock_type = 0;
	Evas_Object *popup = (Evas_Object *) data;

	evas_object_data_set(popup, "selected", "ok");
	app_control_h app_control = (app_control_h)evas_object_data_get(popup, "app-control");
	app_control_add_extra_data(app_control, APP_CONTROL_DATA_SHIFT_WINDOW, "true");

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);

	if (lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD || lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
		app_control_add_extra_data(app_control, "viewtype", "SETTING_PW_TYPE_ENTER_LOCK_TYPE");
		app_control_send_launch_request(app_control, reply_password_enforce_event_handler, popup);
	} else {
		app_control_set_app_id(app_control, "org.tizen.setting-locktype");
		app_control_send_launch_request(app_control, NULL, NULL);
		evas_object_del(popup);
	}
	app_control_destroy(app_control);
}

static int __set_notification(notification_h noti_handle, app_control_h app_control, char *title, char *content)
{
	int ret = NOTIFICATION_ERROR_NONE;

	ret = notification_set_text(noti_handle, NOTIFICATION_TEXT_TYPE_TITLE, __(title), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_text(noti_handle, NOTIFICATION_TEXT_TYPE_CONTENT, __(content), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_display_applist(noti_handle, NOTIFICATION_DISPLAY_APP_ALL);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_image(noti_handle, NOTIFICATION_IMAGE_TYPE_THUMBNAIL, DPM_SYSPOPUP_ICON_PATH);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_launch_option(noti_handle, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, app_control);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	return ret;
}

static void cancel_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *popup = (Evas_Object *) data;
	evas_object_data_set(popup, "selected", "cancel");
	evas_object_del(popup);
	return;
}

static void __default_popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	popup_info_s *info = (popup_info_s *)data;
	char *selected = (char *)evas_object_data_get(obj, "selected");

	if (!strcmp(selected, "cancel")) {
		app_control_h app_control = (app_control_h)evas_object_data_get(obj, "app-control");
		int ret = NOTIFICATION_ERROR_NONE;

		/* create notification */
		notification_h noti_handle = NULL;
		noti_handle = notification_create(NOTIFICATION_TYPE_NOTI);

		ret = __set_notification(noti_handle, app_control, info->noti_title, info->noti_content);
		if (ret != NOTIFICATION_ERROR_NONE) {
			notification_free(noti_handle);
			app_control_destroy(app_control);
			return;
		}

		notification_post(noti_handle);

		notification_free(noti_handle);
		app_control_destroy(app_control);
	}

	ui_app_exit();
}

static Eina_Bool __home_key_cb(void *data, int type, void *event)
{
	Evas_Object *popup = (Evas_Object *)data;
	Evas_Event_Key_Down *ev = event;

	evas_object_data_set(popup, "selected", "cancel");

	if (!strcmp(ev->keyname, "XF86Home"))
		ui_app_exit();

	return EINA_TRUE;
}

static void __create_popup_btn(Evas_Object *popup, char *part, char *btn_text, Evas_Smart_Cb func)
{
	Evas_Object *btn = NULL;

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup");
	elm_object_text_set(btn, __(btn_text));
	elm_object_part_content_set(popup, part, btn);
	evas_object_smart_callback_add(btn, "clicked", func, popup);

	return;
}

void _create_syspopup(const char *id, char *style, const char *status, app_control_h svc)
{
	Evas_Object *popup = NULL;
	Evas_Smart_Cb ok_button_handler = NULL;
	popup_info_s *info = NULL;
	int ret = 0;
	char header[PATH_MAX] = "";
	char body[PATH_MAX] = "";

	if (!strcmp(id, "password-enforce-change"))
		ok_button_handler = password_enforce_event_handler;
	else
		ok_button_handler = default_ok_button_cb;

	info = _get_popup_info(id);
	if (info == NULL)
		return;

	ret = _get_popup_text(id, status, header, body);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup text");
		return;
	}

	create_window("dpm-syspopup");
	popup = elm_popup_add(main_window);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (style != NULL)
		info->style = style;

	elm_object_style_set(popup, info->style);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, main_window);

	if (!strcmp(info->style, "default")) {
		if (strcmp(header, "")) {
			elm_object_part_text_set(popup, "title,text", header);
			elm_object_item_part_text_translatable_set(popup, "title,text", EINA_TRUE);
		}

		if (strcmp(body, ""))
			elm_object_text_set(popup, body);
		elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __default_popup_del_cb, info);
		evas_object_data_set(popup, "app-control", svc);

		if (info->left_btn != NULL) {
			ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, __home_key_cb, popup);

			__create_popup_btn(popup, "button1", info->left_btn, cancel_button_cb);
			__create_popup_btn(popup, "button2", info->right_btn, ok_button_handler);
		} else {
			__create_popup_btn(popup, "button1", info->right_btn, ok_button_handler);

			eext_object_event_callback_del(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb);
			/*[TBD] eext_win_keygrab_set(win, "XF86Home");*/
		}
	} else {
		elm_object_text_set(popup, body);
		elm_popup_timeout_set(popup, 3.0);
		evas_object_smart_callback_add(popup, "timeout", __popup_timeout_cb, NULL);
		evas_object_smart_callback_add(popup, "block,clicked", __block_clicked_cb, NULL);

		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb, NULL);
	}

	evas_object_show(popup);

	return;
}
