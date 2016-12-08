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

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "org.tizen.dpm-syspopup"

#define __(str) dgettext("dpm-syspopup", str)

#define DPM_SYSPOPUP_DEFAULT_STATUS "stop"
#define DPM_SYSPOPUP_ICON_PATH "/usr/share/icons/default/small/org.tizen.dpm-syspopup.png"

typedef struct {
	char *title;
	char *content;
} noti_info_s;

typedef struct {
	const char *id;
	bool text_prefix;
	char *title;
	char *content;
	char *style;
	char *left_btn;
	char *right_btn;
	char *noti_title;
	char *noti_content;
} popup_info_s;

popup_info_s *get_popup_info(const char *id);
int get_popup_text(const char *id, const char *status, char *header, char *body);

void create_syspopup(const char *id, char *style, const char *status, app_control_h svc);

#endif /* __DPM_SYSPOPUP_H__ */
