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

#ifndef __DPM_SYSPOPUP_H__
#define __DPM_SYSPOPUP_H__

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <app.h>
#include <app_common.h>
#include <bundle.h>
#include <system_settings.h>
#include <dlog.h>
#include <Elementary.h>
#include <notification.h>
#include <app_control_internal.h>

#include "widget.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "org.tizen.dpm-syspopup"

#define __(str) dgettext("dpm-syspopup", str)

#define ARRAY_SIZE(_array_) \
        (sizeof(_array_) / sizeof(_array_[0]))

typedef enum {
	DPM_SYSPOPUP_DEFAULT = 0,
	DPM_SYSPOPUP_TOAST,
} viewtype_e;

typedef struct {
        char *key;
        char *value;
} appdata_s;

typedef struct {
        const char *id;
        char *app_id;
        appdata_s *data;
	int data_size;
} appcontrol_s;

typedef struct {
	const char *id;
	char *title;
	char *content;
	const char *image;
} notification_s;

typedef struct {
	const char *id;
	viewtype_e viewtype;
	bool prefix;
	char *header;
	char *body;
	char *left_btn;
	char *right_btn;
} popup_info_s;

app_control_h create_syspopup_app_control(appcontrol_s *list, const char *id);
int create_syspopup_notification(notification_s *list, const char *id, app_control_h app_control);
popup_info_s *get_popup_info(const char *id);

Evas_Object *create_default_popup(Evas_Object *parent, popup_info_s *info, void *user_data);
Evas_Object *create_toast_popup(Evas_Object *parent, popup_info_s *body, void *user_data);
Evas_Object *create_password_enforce_change_popup(Evas_Object *parent, popup_info_s *info, void *user_data);

void create_syspopup(const char *id, void *user_data);

#endif /* __DPM_SYSPOPUP_H__ */
