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

#include "dpm-syspopup.h"

static appcontrol_s *get_appcontrol_info(appcontrol_s *list, const char *id)
{
	while (list->id != NULL) {
		if (!strcmp(list->id, id)) {
			return list;
		}
		list++;
	}
	return NULL;
}

app_control_h create_syspopup_app_control(appcontrol_s *list, const char *id)
{
	int i = 0, ret = 0;
	app_control_h app_control = NULL;
	appcontrol_s *info = NULL;
	appdata_s *appdata = NULL;
	int appdata_size = 0;

	info = get_appcontrol_info(list, id);
	if (info == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "cannot find launch request");
		return NULL;
	}

	ret = app_control_create(&app_control);
	if (ret != APP_CONTROL_ERROR_NONE) {
		return NULL;
	}

	ret = app_control_set_app_id(app_control, info->app_id);
	if (ret != APP_CONTROL_ERROR_NONE) {
		app_control_destroy(app_control);
		return NULL;
	}

	appdata = info->data;
	appdata_size = info->data_size;

	for (i = 0; i < appdata_size; i++) {
		ret = app_control_add_extra_data(app_control, appdata[i].key, appdata[i].value);
		if (ret != APP_CONTROL_ERROR_NONE) {
			app_control_destroy(app_control);
			return NULL;
		}
	}

	return app_control;
}
