/*
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "notification.h"

Notification::Notification()
{
	notification = ::notification_create(NOTIFICATION_TYPE_NOTI);
}

Notification::~Notification()
{
	if (notification)
		::notification_free(notification);
}

void Notification::setTitle(const std::string &title)
{
	::notification_set_text(notification, NOTIFICATION_TEXT_TYPE_TITLE,
				dgettext("dpm-syspopup", title.c_str()), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
}

void Notification::setContent(const std::string &content)
{
	::notification_set_text(notification, NOTIFICATION_TEXT_TYPE_CONTENT,
				dgettext("dpm-syspopup", content.c_str()), NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
}

void Notification::setImage(const std::string &path)
{
	::notification_set_image(notification, NOTIFICATION_IMAGE_TYPE_THUMBNAIL, path.c_str());
}

void Notification::setAppControl(AppControl &appControl)
{
	::notification_set_launch_option(notification, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, appControl);
}

void Notification::post()
{
	::notification_post(notification);
}

void Notification::dispose()
{
	::notification_delete(notification);
}
