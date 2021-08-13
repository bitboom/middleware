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

#include "appcontrol.h"

AppControl::AppControl(app_control_h handle)
	: appControl(nullptr)
{
	if (handle != nullptr) {
		::app_control_clone(&appControl, handle);
	}
	replyEventHandler = [](AppControl *, AppControl *, int) {
	};
}

AppControl::AppControl(const std::string &id)
	: appControl(nullptr)
{
	::app_control_create(&appControl);
	setAppID(id);
}

AppControl::~AppControl()
{
	if (appControl != nullptr)
		::app_control_destroy(appControl);
}

AppControl::operator app_control_h()
{
	return appControl;
}

void AppControl::setAppID(const std::string &id)
{
	::app_control_set_app_id(appControl, id.c_str());
}

void AppControl::setData(const std::string &key, const std::string &data)
{
	::app_control_add_extra_data(appControl, key.c_str(), data.c_str());
}

std::string AppControl::getData(const std::string &key)
{
	std::string data("");
	char *appData = nullptr;

	::app_control_get_extra_data(appControl, key.c_str(), &appData);
	if (appData) {
		data = appData;
		::free(appData);
	}
	return data;
}

void AppControl::launch(void)
{
	::app_control_send_launch_request(appControl, nullptr, nullptr);
}

void AppControl::launch(ReplyHandler &&handler)
{
	replyEventHandler = std::move(handler);
	::app_control_send_launch_request(appControl, replyEventHandlerDispatcher, reinterpret_cast<void *>(this));
}

void AppControl::replyEventHandlerDispatcher(app_control_h ug, app_control_h reply, app_control_result_e result, void *data)
{
	AppControl *appControl = reinterpret_cast<AppControl *>(data);
	AppControl ugControl(ug);
	AppControl replyControl(reply);

	appControl->replyEventHandler(&ugControl, &replyControl, result);
}
