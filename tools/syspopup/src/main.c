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

#include "dpm-syspopup.h"

typedef struct {
	char *id;
	char *style;
	char *status;
	char **user_data;
	int data_size;
} bundle_data_s;

static bundle_data_s bundle_data = {NULL, NULL, NULL, NULL, 0};

static int create_app_control(app_control_h svc)
{
	int i;

	for (i = 0; i < bundle_data.data_size - 1; i++) {
		char *key = NULL;
		char *value = NULL;

		key = bundle_data.user_data[i++];
		value = bundle_data.user_data[i];

		if (!strcmp(key, "app-id")) {
			if (app_control_set_app_id(svc, value) != APP_CONTROL_ERROR_NONE)
				return -1;
		} else {
			if (app_control_add_extra_data(svc, key, value) != APP_CONTROL_ERROR_NONE)
				return -1;
		}
	}

	return 0;
}

static bool app_create(void *data)
{
	return true;
}

static void free_data(void)
{
	int i = 0;

	free(bundle_data.id);
	free(bundle_data.style);
	free(bundle_data.status);

	if (bundle_data.user_data != NULL) {
		for (i = 0; i < bundle_data.data_size; i++) {
			free(bundle_data.user_data[i]);
		}
	}

	return;
}

static void app_control(app_control_h app_control, void *data)
{
	int ret = 0;
	app_control_h svc = NULL;

	ret = app_control_get_extra_data(app_control, "id", &bundle_data.id);
	if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup id");
		ui_app_exit();
		return;
	}

	ret = app_control_get_extra_data(app_control, "viewtype", &bundle_data.style);
	if (ret == APP_CONTROL_ERROR_KEY_NOT_FOUND) {
		bundle_data.style = NULL;
	} else if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup style");
		ui_app_exit();
		return;
	}

	ret = app_control_get_extra_data(app_control, "status", &bundle_data.status);
	if (ret == APP_CONTROL_ERROR_KEY_NOT_FOUND) {
		bundle_data.status = strdup(DPM_SYSPOPUP_DEFAULT_STATUS);
	} else if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup status");
		ui_app_exit();
		return;
	}

	ret = app_control_get_extra_data_array(app_control, "user-data", &bundle_data.user_data, &bundle_data.data_size);
	if (ret != APP_CONTROL_ERROR_KEY_NOT_FOUND && ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup user data");
		ui_app_exit();
		return;
	}

	ret = app_control_create(&svc);
	if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create app_control handler");
		ui_app_exit();
		return;
	}

	if (create_app_control(svc) != 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set app_control handler");
		app_control_destroy(svc);
		ui_app_exit();
		return;
	}

	create_syspopup(bundle_data.id, bundle_data.style, bundle_data.status, svc);

	return;
}

static void app_terminate(void *data)
{
	free_data();
	return;
}

static void app_pause(void *data)
{
	return;
}

static void app_resume(void *data)
{
	return;
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	int ret = 0;
	char *locale = NULL;

	ret = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get language");
		return;
	}

	elm_language_set(locale);
	free(locale);
	return;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, NULL);

	ret = ui_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);

	return ret;
}
