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

#ifndef __DPM_SYSPOPUP_APPCONTROL_H__
#define __DPM_SYSPOPUP_APPCONTROL_H__

#include <functional>
#include <app_control_internal.h>

class AppControl {
public:
	typedef std::function<void(AppControl *, AppControl *, int)> ReplyHandler;
	AppControl(app_control_h handle);
	AppControl(const std::string &id);
	~AppControl();

	operator app_control_h();

	void setAppID(const std::string &id);
	void setData(const std::string &key, const std::string &data);
	std::string getData(const std::string &key);

	void launch();
	void launch(ReplyHandler &&handler);
	static void replyEventHandlerDispatcher(app_control_h ug, app_control_h reply, app_control_result_e result, void *data);
public:
	ReplyHandler replyEventHandler;
private:
	app_control_h appControl;
};

#endif
