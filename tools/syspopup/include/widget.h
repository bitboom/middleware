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

#ifndef __DPM_SYSPOPUP_WIDGET_H__
#define __DPM_SYSPOPUP_WIDGET_H__

#include <app.h>
#include <app_common.h>
#include <Elementary.h>

Evas_Object *create_window(const char *pkg_name);
Evas_Object *create_popup(Evas_Object *parent, const char *style);
Evas_Object *create_button(Evas_Object *parent, const char *style, const char *text, Evas_Smart_Cb callback, void *data);
Evas_Object *create_image(Evas_Object *parent, const char *path);

#endif /* __DPM_SYSPOPUP_WIDGET_H__ */
