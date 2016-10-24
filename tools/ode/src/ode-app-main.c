/*
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ode-app.h"

#define APP_SETTING_LOCKTYPE "org.tizen.setting-locktype"
#define APP_SETTING_PASSWORD "org.tizen.setting-password"

static void update_next_button_status(appdata_s* ad)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "update_next_button_status start");
	if (ad->device_info.next_button == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[Encrypt Device] next button is null");
		return;
	}

	if (ad->view_type == ENCRYPT_DEVICE) {
		if (ad->device_info.battery_status && ad->device_info.charger_status && ad->device_info.locktype_status) {
			elm_object_disabled_set(ad->device_info.next_button, EINA_FALSE);
		} else {
			elm_object_disabled_set(ad->device_info.next_button, EINA_TRUE);
		}
	} else if (ad->view_type == DECRYPT_DEVICE) {
		if (ad->device_info.battery_status && ad->device_info.charger_status) {
			elm_object_disabled_set(ad->device_info.next_button, EINA_FALSE);
		} else {
			elm_object_disabled_set(ad->device_info.next_button, EINA_TRUE);
		}
	} else if (ad->view_type == ENCRYPT_SD_CARD) {
		if (ad->device_info.sdcard_status) {
			elm_object_disabled_set(ad->device_info.next_button, EINA_FALSE);
		} else {
			elm_object_disabled_set(ad->device_info.next_button, EINA_TRUE);
		}
	}
}

static void _next_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "_next_btn_clicked_cb start");

	appdata_s* ad = (appdata_s*) data;

	switch (ad->view_type) {
	case ENCRYPT_DEVICE:
		dpm_encryption_device_confirm_create_view(ad);
		break;
	case DECRYPT_DEVICE:
		dpm_decryption_device_confirm_create_view(ad);
		break;
	case ENCRYPT_SD_CARD:
		dpm_encryption_sd_card_confirm_create_view(ad);
		break;
	case DECRYPT_SD_CARD:
		dpm_decryption_sd_card_confirm_create_view(ad);
		break;
	}
}

static void set_next_btn_layout(Evas_Object* layout, appdata_s* ad, const char* btn_text)
{
	Evas_Object* btn_layout;
	Evas_Object* next_btn;

	btn_layout = dpm_encryption_create_layout(layout, ad->edj_path, "next_button_layout");

	next_btn = dpm_encryption_create_button(btn_layout, btn_text, "bottom");
	ad->device_info.next_button = next_btn;
	elm_object_part_content_set(btn_layout, "next_button", next_btn);

	update_next_button_status(ad);
	evas_object_smart_callback_add(next_btn, "clicked", _next_btn_clicked_cb, ad);

	elm_object_part_content_set(layout, "bottom_layout", btn_layout);
	return;
}

static void _launch_password_ug_cb(app_control_h ug, app_control_h reply, app_control_result_e result, void* data)
{
	if (!result) {
		char* string_result = NULL;
		char* current = NULL;
		app_control_get_extra_data(reply, "result", &string_result);
		if (string_result == NULL)
			return;

		int ret_int = 0;
		if (strcmp(string_result, "Cancel") == 0)
			ret_int = 0;
		else if (strcmp(string_result, "SETTING_PW_TYPE_ENTER_LOCK_TYPE") == 0 || strcmp(string_result, "SETTING_PW_TYPE_VERIFY_FP_ALT_PASSWORD") == 0)
			ret_int = 1;

		free(string_result);

		if (ret_int == 1) {
			app_control_h app_ctl;
			if (app_control_create(&app_ctl)) {
				dlog_print(DLOG_ERROR, LOG_TAG, "app_control_create failed");
				return;
			}
			app_control_get_extra_data(reply, "current", &current);
			if (current) {
				app_control_add_extra_data(app_ctl, "current", current);
				free(current);
			}
			app_control_set_app_id(app_ctl, APP_SETTING_LOCKTYPE);
			app_control_set_launch_mode(app_ctl, APP_CONTROL_LAUNCH_MODE_GROUP);
			app_control_send_launch_request(app_ctl, NULL, NULL);
			app_control_destroy(app_ctl);
		}
	}
}

static void _locktype_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	/* launch lock type menu */
	dlog_print(DLOG_DEBUG, LOG_TAG, "locktype button clicked");
	appdata_s*ad = (appdata_s*) data;

	app_control_h app_ctl = NULL;
	int ret;
	int lock_type = 0;

	ret = app_control_create(&app_ctl);
	if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "app_control_create failed");
		return;
	}

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);

	if (lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD || lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
		app_control_add_extra_data(app_ctl, "viewtype", "SETTING_PW_TYPE_ENTER_LOCK_TYPE");
		app_control_set_app_id(app_ctl, APP_SETTING_PASSWORD);
		app_control_set_launch_mode(app_ctl, APP_CONTROL_LAUNCH_MODE_GROUP);
		app_control_send_launch_request(app_ctl, _launch_password_ug_cb, NULL);
		app_control_destroy(app_ctl);

	} else {
		app_control_set_app_id(app_ctl, APP_SETTING_LOCKTYPE);
		app_control_set_launch_mode(app_ctl, APP_CONTROL_LAUNCH_MODE_GROUP);
		app_control_send_launch_request(app_ctl, NULL, NULL);
		app_control_destroy(app_ctl);
	}

	update_next_button_status(ad);
}

static void _sdcardpw_create_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	dlog_print(DLOG_ERROR, LOG_TAG, "sdcardpw create btn cb start");
}

static void _sdcardpw_change_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	dlog_print(DLOG_ERROR, LOG_TAG, "sdcardpw change btn cb start");
}

static void _sdcardpw_delete_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	dlog_print(DLOG_ERROR, LOG_TAG, "sdcardpw delete btn cb start");
}

static void set_locktype_btn_layout(Evas_Object* layout, appdata_s* ad, const char* btn_text)
{
	Evas_Object* btn_layout;
	Evas_Object* locktype_btn;

	btn_layout = dpm_encryption_create_layout(layout, ad->edj_path, "locktype_button_layout");

	locktype_btn = dpm_encryption_create_button(btn_layout, btn_text, "default");
	elm_object_part_content_set(btn_layout, "locktype_button", locktype_btn);
	evas_object_smart_callback_add(locktype_btn, "clicked", _locktype_btn_clicked_cb, ad);

	elm_object_part_content_set(layout, "locktype_button_content", btn_layout);
	return;
}

static void set_sdcard_pw_btn_layout(Evas_Object* layout, appdata_s* ad)
{
	Evas_Object* btn_layout;
	Evas_Object* createpw_btn, *changepw_btn, *deletepw_btn;

	btn_layout = dpm_encryption_create_layout(layout, ad->edj_path, "sdcard_pw_button_layout");

	createpw_btn = dpm_encryption_create_button(btn_layout, IDS_ST_BUTTON_CREATE_PASSWORD, "default");
	changepw_btn = dpm_encryption_create_button(btn_layout, IDS_ST_BUTTON_CHANGE_PW_ABB, "default");
	deletepw_btn = dpm_encryption_create_button(btn_layout, IDS_ST_BUTTON_DELETE_PW_ABB, "default");

	elm_object_part_content_set(btn_layout, "create_pw_button", btn_layout);
	elm_object_part_content_set(btn_layout, "change_pw_button", btn_layout);
	elm_object_part_content_set(btn_layout, "delete_pw_button", btn_layout);

	evas_object_smart_callback_add(createpw_btn, "clicked", _sdcardpw_create_btn_clicked_cb, NULL);
	evas_object_smart_callback_add(changepw_btn, "clicked", _sdcardpw_change_btn_clicked_cb, NULL);
	evas_object_smart_callback_add(deletepw_btn, "clicked", _sdcardpw_delete_btn_clicked_cb, NULL);

	elm_object_part_content_set(layout, "sd_card_btn", btn_layout);
	return;
}

static void set_icon(appdata_s* ad, int type, char* part)
{
	Evas_Object* icon_check, *icon_bar = NULL;
	int ret;
	if (ad->dpm_encryption_layout == NULL || ad->icon_check_path == NULL || ad->icon_bar_path == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "invalid parameter");
		return;
	}
	if (type == ICON_CHECK) {
		icon_check = elm_image_add(ad->dpm_encryption_layout);
		if (icon_check == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "elm_image_add failed");
			return;
		}
		elm_image_file_set(icon_check, ad->icon_check_path, NULL);
		evas_object_color_set(icon_check, 153, 153, 153, 255);
		evas_object_size_hint_align_set(icon_check, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(icon_check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		ret = elm_layout_content_set(ad->dpm_encryption_layout, part, icon_check);
		if (!ret) {
			dlog_print(DLOG_ERROR, LOG_TAG, "elm_layout_content_set failed");
			return;
		}
		evas_object_show(icon_check);
	} else if (type == ICON_BAR) {
		icon_bar = elm_image_add(ad->dpm_encryption_layout);
		if (icon_bar == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "elm_image_add failed");
			return;
		}
		elm_image_file_set(icon_bar, ad->icon_bar_path, NULL);
		evas_object_color_set(icon_bar, 0, 0, 0, 255);
		evas_object_size_hint_align_set(icon_bar, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(icon_bar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		ret = elm_layout_content_set(ad->dpm_encryption_layout, part, icon_bar);
		if (!ret) {
			dlog_print(DLOG_ERROR, LOG_TAG, "elm_layout_content_set failed");
			return;
		}
		evas_object_show(icon_bar);
	}
}

static int battery_status_update(appdata_s* ad)
{
	bool connected;
	int battery_pct, ret;
	dlog_print(DLOG_DEBUG, LOG_TAG, "battery_status_update start ");

	if (ad->dpm_encryption_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "encrypt layout is null");
		return -1;
	}

	/* get battery percent info */
	ret = device_battery_get_percent(&battery_pct);
	if (ret != DEVICE_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "device_battery_get_percent failed %d", ret);
		return -1;
	}

	if (battery_pct > NEEDED_BATTERY_PERCENT) {
		set_icon(ad, ICON_CHECK, "battery_icon");
		ad->device_info.battery_status = 1;
	} else {
		set_icon(ad, ICON_BAR, "battery_icon");
		ad->device_info.battery_status = 0;
	}

	/* get charger connected info */
	ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_CHARGER_CONNECTED, &connected);
	if (ret != RUNTIME_INFO_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "get runtime info fail");
		return -1;
	}
	if (connected) {
		set_icon(ad, ICON_CHECK, "charge_icon");
		ad->device_info.charger_status = 1;
	} else {
		set_icon(ad, ICON_BAR, "charge_icon");
		ad->device_info.charger_status = 0;
	}

	update_next_button_status(ad);
	return 0;
}

static int sdcard_status_update(appdata_s* ad)
{
	int sdcard_status;

	if (ad->dpm_encryption_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "encrypt layout is null");
		return -1;
	}

	vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &sdcard_status);
	if (sdcard_status != VCONFKEY_SYSMAN_MMC_MOUNTED) {
		set_icon(ad, ICON_BAR, "sd_card_icon");
		ad->device_info.sdcard_status = 0;
	} else {
		/* available */
		set_icon(ad, ICON_CHECK, "sd_card_icon");
		ad->device_info.sdcard_status = 1;
	}

	update_next_button_status(ad);
	return 0;
}

static int locktype_status_update(appdata_s* ad)
{
	int lock_type;

	if (ad->dpm_encryption_layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "encrypt layout is null");
		return -1;
	}

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);
	if (lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD || lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
		/* available */
		set_icon(ad, ICON_CHECK, "locktype_icon");
		ad->device_info.locktype_status = 1;
	} else {
		/* unavailable */
		set_icon(ad, ICON_BAR, "locktype_icon");
		ad->device_info.locktype_status = 0;
	}
	update_next_button_status(ad);
	return 0;
}

static void battery_charger_changed_cb(runtime_info_key_e key, void* data)
{
	appdata_s* ad = (appdata_s*) data;
	if (key == RUNTIME_INFO_KEY_CHARGER_CONNECTED)
		battery_status_update(ad);
}

void battery_changed_cb(device_callback_e type, void* value, void* user_data)
{
	appdata_s* ad = (appdata_s*) user_data;
	if (type == DEVICE_CALLBACK_BATTERY_CAPACITY)
		battery_status_update(ad);
}

void sdcard_status_changed_cb(keynode_t* node, void* data)
{
	appdata_s* ad = (appdata_s*) data;
	sdcard_status_update(ad);
}

void locktype_status_changed_cb(keynode_t* node, void* data)
{
	appdata_s* ad = (appdata_s*) data;
	locktype_status_update(ad);
}

static int battery_cb_register(appdata_s* ad)
{
	int ret;

	/* register runtime callback : connected charger */
	ret = runtime_info_set_changed_cb(RUNTIME_INFO_KEY_CHARGER_CONNECTED, battery_charger_changed_cb, ad);
	if (ret != RUNTIME_INFO_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "runtime_info_set_changed_cb failed: %d", ret);
		return -1;
	}

	/* register runtime callback : battery percent */
	ret = device_add_callback(DEVICE_CALLBACK_BATTERY_CAPACITY, battery_changed_cb, ad);
	if (ret != DEVICE_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "device_add_callback failed: %d", ret);
		return -1;
	}

	return 0;
}

static int locktype_cb_register(appdata_s* ad)
{
	int ret;

	/* register vconf notify for locktype */
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, locktype_status_changed_cb, ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "runtime_info_set_changed_cb failed: %d", ret);
		return -1;
	}

	return 0;
}

void _back_btn_clicked_cb(void* data, Evas_Object* obj, void* event_info)
{
	ui_app_exit();
}

static Eina_Bool _pop_cb(void* data, Elm_Object_Item* it)
{
	ui_app_exit();
	return EINA_FALSE;
}

void sw_back_btn_set(Evas_Object* obj, const char* title_label, Evas_Object* content)
{
	Evas_Object* back_btn = NULL;

	back_btn = elm_button_add(obj);
	elm_object_style_set(back_btn, "naviframe/back_btn/default");
	evas_object_smart_callback_add(back_btn, "clicked", _back_btn_clicked_cb, obj);

	Elm_Object_Item *nf_it = elm_naviframe_item_push(obj, title_label, back_btn, NULL, content, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, _pop_cb, NULL);
}

void dpm_encryption_create_view(appdata_s* ad)
{
	Evas_Object* navi_bar = ad->navi_bar;
	Evas_Object* base_layout = NULL;
	Evas_Object* encrypt_msg1, *encrypt_msg2, *encrypt_msg3 = NULL;
	Evas_Object* battery_txt, *charge_txt, *locktype_txt = NULL;
	Evas_Object* encryption_layout = NULL;
	Evas_Textblock_Style* text_st;

	int ret;

	dlog_print(DLOG_DEBUG, LOG_TAG, "start dpm_encryption_create_view");

	if (navi_bar == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "navi_bar is null");
		return;
	}

	base_layout = dpm_encryption_create_layout(navi_bar, ad->edj_path, "base_layout");
	encryption_layout = dpm_encryption_create_layout(base_layout, ad->edj_path, "Encrypt_device_layout");
	ad->dpm_encryption_layout = encryption_layout;

	text_st = evas_textblock_style_new();
	evas_textblock_style_set(text_st, MSG_TEXT_STYLE);

	encrypt_msg1 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_ENCRYPTING_MSG_1, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_1", encrypt_msg1);
	encrypt_msg2 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_ENCRYPTING_MSG_2, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_2", encrypt_msg2);
	encrypt_msg3 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_TO_ENCRYPTION_YOUR_DEVICE_C_ABB, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_3", encrypt_msg3);

	set_next_btn_layout(base_layout, ad, IDS_ST_BUTTON_NEXT);

	/* get battery info */
	ret = battery_status_update(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery_status_update failed");
		return;
	}

	ret = battery_cb_register(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery_cb_register failed");
		return;
	}

	/* get locktype */
	ret = locktype_status_update(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "locktype_status_update failed");
		return;
	}

	ret = locktype_cb_register(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "locktype_cb_register failed");
		return;
	}

	/* set layout text */
	battery_txt = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_CHARGE_THE_BATTERY_TO_AT_LEAST_PDP, text_st);
	elm_object_part_content_set(encryption_layout, "battery_text", battery_txt);
	charge_txt = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_CONNECT_THE_DEVICE_TO_A_CHARGER, text_st);
	evas_object_size_hint_weight_set(charge_txt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(encryption_layout, "charge_text", charge_txt);
	locktype_txt = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_SET_THE_SCREEN_LOCK_TYPE_AS_PATTERN_PIN_OR_PASSWORD, text_st);
	elm_object_part_content_set(encryption_layout, "locktype_text", locktype_txt);

	evas_textblock_style_free(text_st);

	set_locktype_btn_layout(encryption_layout, ad, IDS_ST_BUTTON_SCREEN_LOCK_TYPE_ABB);

	elm_object_part_content_set(base_layout, "content_layout", encryption_layout);

	sw_back_btn_set(navi_bar, IDS_ST_HEADER_ENCRYPT_DEVICE, base_layout);
}

void dpm_decryption_create_view(appdata_s* ad)
{
	Evas_Object* navi_bar = ad->navi_bar;
	Evas_Object* base_layout = NULL;
	Evas_Object* decrypt_msg1, *decrypt_msg2, *decrypt_msg3 = NULL;
	Evas_Object* battery_txt, *charge_txt = NULL;
	Evas_Object* decryption_layout = NULL;
	Evas_Textblock_Style* text_st;

	int ret;

	dlog_print(DLOG_DEBUG, LOG_TAG, "start dpm_decryption_create_view");

	if (navi_bar == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "navi_bar is null");
		return;
	}

	base_layout = dpm_encryption_create_layout(navi_bar, ad->edj_path, "base_layout");
	decryption_layout = dpm_encryption_create_layout(base_layout, ad->edj_path, "Decrypt_device_layout");
	ad->dpm_encryption_layout = decryption_layout;

	text_st = evas_textblock_style_new();
	evas_textblock_style_set(text_st, MSG_TEXT_STYLE);

	decrypt_msg1 = dpm_encryption_create_textblock(decryption_layout, IDS_ST_BODY_DECRYPTING_MSG_1, text_st);
	elm_object_part_content_set(decryption_layout, "msg_content_1", decrypt_msg1);
	decrypt_msg2 = dpm_encryption_create_textblock(decryption_layout, IDS_ST_BODY_DECRYPTING_MSG_2, text_st);
	elm_object_part_content_set(decryption_layout, "msg_content_2", decrypt_msg2);
	decrypt_msg3 = dpm_encryption_create_textblock(decryption_layout, IDS_ST_HEADER_TO_DECRYPT_YOUR_DEVICE_C_ABB, text_st);
	elm_object_part_content_set(decryption_layout, "msg_content_3", decrypt_msg3);

	set_next_btn_layout(base_layout, ad, IDS_ST_BUTTON_NEXT);

	/* get battery info */
	ret = battery_status_update(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery_status_update failed");
		return;
	}

	ret = battery_cb_register(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "battery_cb_register failed");
		return;
	}

	/* set layout text */
	battery_txt = dpm_encryption_create_textblock(decryption_layout, IDS_ST_BODY_CHARGE_THE_BATTERY_TO_AT_LEAST_PDP, text_st);
	elm_object_part_content_set(decryption_layout, "battery_text", battery_txt);
	charge_txt = dpm_encryption_create_textblock(decryption_layout, IDS_ST_BODY_CONNECT_THE_DEVICE_TO_A_CHARGER, text_st);
	elm_object_part_content_set(decryption_layout, "charge_text", charge_txt);

	evas_textblock_style_free(text_st);

	elm_object_part_content_set(base_layout, "content_layout", decryption_layout);

	sw_back_btn_set(navi_bar, IDS_ST_HEADER_DECRYPT_DEVICE, base_layout);
}

void dpm_encryption_sd_card_create_view(appdata_s* ad)
{
	Evas_Object* navi_bar = ad->navi_bar;
	Evas_Object* base_layout = NULL;
	Evas_Object* encrypt_msg1, *encrypt_msg2, *encrypt_msg3, *encrypt_msg4 = NULL;
	Evas_Object* sdcard_txt = NULL;
	/* Evas_Object* password_txt = NULL; */
	Evas_Object* encryption_layout = NULL;
	Evas_Textblock_Style* text_st;

	int ret;

	dlog_print(DLOG_DEBUG, LOG_TAG, "start dpm_encryption_create_view");

	if (navi_bar == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "navi_bar is null");
		return;
	}

	base_layout = dpm_encryption_create_layout(navi_bar, ad->edj_path, "base_layout");
	encryption_layout = dpm_encryption_create_layout(base_layout, ad->edj_path, "Encrypt_sd_card_layout");
	ad->dpm_encryption_layout = encryption_layout;

	text_st = evas_textblock_style_new();
	evas_textblock_style_set(text_st, MSG_TEXT_STYLE);

	encrypt_msg1 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_1, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_1", encrypt_msg1);
	encrypt_msg2 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_2, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_2", encrypt_msg2);
	encrypt_msg3 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_3, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_3", encrypt_msg3);
	encrypt_msg4 = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_TO_ENCRYPT_YOUR_SD_CARD_C, text_st);
	elm_object_part_content_set(encryption_layout, "msg_content_4", encrypt_msg4);

	set_next_btn_layout(base_layout, ad, IDS_ST_BUTTON_NEXT);

	/* get sdcard info */
	ret = sdcard_status_update(ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "sdcard_status_update failed");
		return;
	}

	/* register vconf notify callback : sdcard status */
	ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_STATUS, sdcard_status_changed_cb, ad);
	if (ret != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "runtime_info_set_changed_cb failed: %d", ret);
		return;
	}

	/* set layout text */
	sdcard_txt = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_INSERT_THE_SD_CARD_ABB, text_st);
	elm_object_part_content_set(encryption_layout, "sd_card_text", sdcard_txt);
	/*password_txt = dpm_encryption_create_textblock(encryption_layout, IDS_ST_BODY_CREATE_AN_SD_CARD_PASSWORD, text_st);
	   elm_object_part_content_set(encryption_layout, "charge_text", password_txt); */

	evas_textblock_style_free(text_st);

	set_sdcard_pw_btn_layout(encryption_layout, ad);

	/* to do : set password button */

	elm_object_part_content_set(base_layout, "content_layout", encryption_layout);

	sw_back_btn_set(navi_bar, IDS_ST_HEADER_ENCRYPT_SD_CARD, base_layout);
}

void dpm_decryption_sd_card_create_view(appdata_s* ad)
{
	Evas_Object* navi_bar = ad->navi_bar;
	Evas_Object* base_layout = NULL;
	Evas_Object* decrypt_msg = NULL;
	Evas_Object* decryption_layout = NULL;
	Evas_Textblock_Style* text_st;

	dlog_print(DLOG_DEBUG, LOG_TAG, "start dpm_decryption_sd_card_create_view");

	if (navi_bar == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "navi_bar is null");
		return;
	}

	base_layout = dpm_encryption_create_layout(navi_bar, ad->edj_path, "base_layout");
	decryption_layout = dpm_encryption_create_layout(base_layout, ad->edj_path, "Decrypt_sd_card_layout");
	ad->dpm_encryption_layout = decryption_layout;

	text_st = evas_textblock_style_new();
	evas_textblock_style_set(text_st, MSG_TEXT_STYLE);

	decrypt_msg = dpm_encryption_create_textblock(decryption_layout, IDS_ST_BODY_SD_CARD_DECRYPTION_MSG, text_st);
	elm_object_part_content_set(decryption_layout, "msg_content", decrypt_msg);

	evas_textblock_style_free(text_st);

	/* to do : set change password button */

	elm_object_part_content_set(base_layout, "content_layout", decryption_layout);

	set_next_btn_layout(base_layout, ad, IDS_ST_BUTTON_NEXT);
	sw_back_btn_set(navi_bar, IDS_ST_HEADER_ENCRYPT_SD_CARD, base_layout);
}
