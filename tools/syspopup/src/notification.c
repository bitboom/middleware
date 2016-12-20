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

static notification_s *get_notification_info(notification_s *list, const char *id)
{
	while (list->id != NULL) {
		if (!strcmp(list->id, id)) {
			return list;
		}
		list++;
	}
	return NULL;
}

static int set_notification(notification_h noti_handle, app_control_h app_control, notification_s *info)
{
	int ret = NOTIFICATION_ERROR_NONE;

	ret = notification_set_text(noti_handle, NOTIFICATION_TEXT_TYPE_TITLE, __(info->title), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_text(noti_handle, NOTIFICATION_TEXT_TYPE_CONTENT, __(info->content), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_display_applist(noti_handle, NOTIFICATION_DISPLAY_APP_ALL);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	ret = notification_set_image(noti_handle, NOTIFICATION_IMAGE_TYPE_THUMBNAIL, info->image);
	if (ret != NOTIFICATION_ERROR_NONE)
		return -1;

	if (app_control) {
		ret = notification_set_launch_option(noti_handle, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, app_control);
		if (ret != NOTIFICATION_ERROR_NONE)
			return -1;
	}
	return ret;
}

int create_syspopup_notification(notification_s *list, const char *id, app_control_h app_control)
{
	int ret = 0;
	notification_s *info = NULL;
	notification_h handle = NULL;

	info = get_notification_info(list, id);
	if (info == NULL) {
		return -1;
	}

	handle = notification_create(NOTIFICATION_TYPE_NOTI);
	if (handle == NULL) {
		return -1;
	}

	ret = set_notification(handle, app_control, info);
	if (ret != 0) {
		notification_free(handle);
		return -1;
	}

	ret = notification_post(handle);
	if (ret != NOTIFICATION_ERROR_NONE) {
		return -1;
	}

	notification_free(handle);
	return 0;
}
