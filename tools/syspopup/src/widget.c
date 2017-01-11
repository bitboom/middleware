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

#include <efl_util.h>

#include "dpm-syspopup.h"

static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

Evas_Object *create_window(const char *pkg_name)
{
	Evas_Object *window = NULL;
	int rots[] = {0, 90, 180, 270};

	window = elm_win_add(NULL, pkg_name, ELM_WIN_NOTIFICATION);
	efl_util_set_notification_window_level(window, EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT);

	if (elm_win_wm_rotation_supported_get(window))
		elm_win_wm_rotation_available_rotations_set(window, rots, 4);

	elm_win_title_set(window, pkg_name);
	elm_win_borderless_set(window, EINA_TRUE);
	elm_win_alpha_set(window, EINA_TRUE);
	elm_win_autodel_set(window, EINA_TRUE);

	evas_object_smart_callback_add(window, "delete,request", win_delete_request_cb, NULL);
	evas_object_show(window);

	return window;
}

Evas_Object *create_popup(Evas_Object *parent, const char *style)
{
	Evas_Object *popup = NULL;

	popup = elm_popup_add(parent);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (style)
		elm_object_style_set(popup, style);

	return popup;
}

Evas_Object *create_button(Evas_Object *parent, const char *style, const char *text, Evas_Smart_Cb callback, void *data)
{
	Evas_Object *button = NULL;

	button = elm_button_add(parent);
	if (style)
		elm_object_style_set(button, style);
	if (text)
		elm_object_text_set(button, text);
	if (callback)
		evas_object_smart_callback_add(button, "clicked", callback, data);

	return button;
}

Evas_Object *create_image(Evas_Object *parent, const char *path)
{
	Evas_Object *image = NULL;

	if (path == NULL) {
		return NULL;
	}

	image = elm_image_add(parent);
	elm_image_file_set(image, path, NULL);
	evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(image);

	return image;
}
