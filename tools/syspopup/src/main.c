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

static bool app_create(void *data)
{
	return true;
}

static void app_control(app_control_h app_control, void *data)
{
	int ret = 0;
	char *id = NULL;
	char *user_data = NULL;

	ret = app_control_get_extra_data(app_control, "id", &id);
	if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get popup id");
		ui_app_exit();
		return;
	}

	ret = app_control_get_extra_data(app_control, "user-data", &user_data);
	if (ret == APP_CONTROL_ERROR_KEY_NOT_FOUND) {
		user_data = NULL;
	} else if (ret != APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get user-data");
		free(id);
		ui_app_exit();
		return;
	}

	create_syspopup(id, user_data);

	free(id);
	free(user_data);

	return;
}

static void app_terminate(void *data)
{
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
