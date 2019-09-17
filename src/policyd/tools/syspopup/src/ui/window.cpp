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

#include "window.h"

Window::Window(const std::string &packageName)
	: Widget("Window", ::elm_win_add(NULL, packageName.c_str(), ELM_WIN_NOTIFICATION))
{
	::elm_win_title_set(evasObject(this), packageName.c_str());
	setRotate();
	setConfig();
	show();
}

Window::~Window()
{
}

void Window::setRotate()
{
	int rots[] = {0, 90, 180, 270};

	if (::elm_win_wm_rotation_supported_get(evasObject(this)))
		::elm_win_wm_rotation_available_rotations_set(evasObject(this), rots, 4);
}

void Window::setConfig()
{
	::efl_util_set_notification_window_level(evasObject(this), EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT);
	::elm_win_borderless_set(evasObject(this), EINA_TRUE);
	::elm_win_alpha_set(evasObject(this), EINA_TRUE);
	::elm_win_autodel_set(evasObject(this), EINA_TRUE);
}
