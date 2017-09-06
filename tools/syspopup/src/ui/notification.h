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

#ifndef __DPM_SYSPOPUP_NOTIFICATION_H__
#define __DPM_SYSPOPUP_NOTIFICATION_H__

#include <notification.h>
#include "widget.h"
#include "appcontrol.h"

class Notification {
public:
	Notification();
	~Notification();

	void setTitle(const std::string &title);
	void setContent(const std::string &content);
	void setImage(const std::string &path);
	void setAppControl(AppControl &appcontrol);
	void post();
	void dispose();
private:
	notification_h notification;
};

#endif
