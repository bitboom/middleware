/*
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

static void popup_timeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
	return;
}

static void block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
	return;
}

static void popup_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

Evas_Object *create_toast_popup(Evas_Object *parent, popup_info_s *info, void *user_data)
{
	Evas_Object *popup = NULL;
	char body[PATH_MAX] = "";
	char *lp_text = NULL;

	if (info->prefix) {
		lp_text =  __("IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
		snprintf(body, PATH_MAX, lp_text, __(info->body));
	} else {
		snprintf(body, PATH_MAX, "%s", __(info->body));
	}

	popup = create_popup(parent, "toast/circle");
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_part_text_set(popup, "elm.text", body);
	elm_popup_timeout_set(popup, 3.0);
	evas_object_smart_callback_add(popup, "timeout", popup_timeout_cb, NULL);
	evas_object_smart_callback_add(popup, "block,clicked", block_clicked_cb, NULL);
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, popup_deleted_cb, NULL);

	return popup;
}
