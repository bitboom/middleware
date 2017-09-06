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

#include "application.h"

Application::Application(const std::string &name)
	: packageName(name), appControl(nullptr)
{
}

Application::~Application()
{
}

bool Application::createDispatcher(void *data)
{
	Application *application = reinterpret_cast<Application *>(data);
	return application->onCreate();
}

void Application::terminateDispatcher(void *data)
{
	Application *application = reinterpret_cast<Application *>(data);
	return application->onTerminate();
}

void Application::pauseDispatcher(void *data)
{
	Application *application = reinterpret_cast<Application *>(data);
	return application->onPause();
}

void Application::resumeDispatcher(void *data)
{
	Application *application = reinterpret_cast<Application *>(data);
	return application->onResume();
}

void Application::appControlDispatcher(app_control_h app_control, void *data)
{
	Application *application = reinterpret_cast<Application *>(data);
	application->appControl.reset(new AppControl{app_control});

	return application->onAppControl();
}

void Application::languageChangedDispatcher(app_event_info_h event_info, void *user_data)
{
	int ret = 0;
	char *locale = NULL;

	ret = ::system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		throw runtime::Exception("failed to get language");
	}

	::elm_language_set(locale);
	::free(locale);
}

int Application::run(int argc, char *argv[])
{
	int ret = 0;
	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = createDispatcher;
	event_callback.terminate = terminateDispatcher;
	event_callback.pause = pauseDispatcher;
	event_callback.resume = resumeDispatcher;
	event_callback.app_control = appControlDispatcher;

	try {
		::ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
						APP_EVENT_LANGUAGE_CHANGED,
						languageChangedDispatcher, NULL);

		ret = ::ui_app_main(argc, argv, &event_callback, this);
		if (ret != APP_ERROR_NONE)
			throw runtime::Exception("app_main() is failed");
	} catch(runtime::Exception &e) {
		dlog_print(DLOG_ERROR, packageName.c_str(), "%s", e.what());
		::ui_app_exit();
	}

	return ret;
}

bool Application::onCreate()
{
	return true;
}

void Application::onTerminate()
{
}

void Application::onPause()
{
}

void Application::onResume()
{
}

void Application::onAppControl()
{
}
