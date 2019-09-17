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

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <memory>

#include <app.h>
#include <app_common.h>
#include <Elementary.h>
#include <efl_util.h>

#include <dlog.h>
#include <system_info.h>
#include <system_settings.h>
#include <klay/exception.h>

#include "ui/appcontrol.h"

class Application {
public:
	Application(const std::string &name);
	virtual ~Application();

	virtual int run(int argc, char *argv[]);

protected:
	virtual bool onCreate();
	virtual void onResume();
	virtual void onPause();
	virtual void onTerminate();
	virtual void onAppControl();

	std::string packageName;
	std::unique_ptr<AppControl> appControl;
private:
	static bool createDispatcher(void *data);
	static void resumeDispatcher(void *data);
	static void pauseDispatcher(void *data);
	static void terminateDispatcher(void *data);
	static void appControlDispatcher(app_control_h app_control, void *data);
	static void languageChangedDispatcher(app_event_info_h event_info, void* user_data);
};

#endif /*__APPLICATION_H__*/
