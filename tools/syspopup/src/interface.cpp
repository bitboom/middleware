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

#include "interface.h"

SyspopupInterface::SyspopupInterface()
	: prefix(0), title(""), content(""), popup(nullptr), window(nullptr)
{
}

SyspopupInterface::SyspopupInterface(const std::string &contentText)
	: prefix(0), title(""), content(contentText), popup(nullptr), window(nullptr)
{
}

SyspopupInterface::SyspopupInterface(int prefixEnabled, const std::string &contentText)
	: prefix(prefixEnabled), title(""), content(contentText), popup(nullptr), window(nullptr)
{
}

SyspopupInterface::SyspopupInterface(const std::string &titleText, const std::string &contentText)
	: prefix(0), title(titleText), content(contentText), popup(nullptr), window(nullptr)
{
}

SyspopupInterface::~SyspopupInterface()
{
	if (popup != nullptr) {
		popup->dispose();
	}

	if (window)
		delete window;
}

int SyspopupInterface::getProfile()
{
	char *profileName = nullptr;
	::system_info_get_platform_string("http://tizen.org/feature/profile", &profileName);

	switch (*profileName) {
	case 'm':
	case 'M':
		::free(profileName);
		return Profile::Mobile;
		break;
	case 'w':
	case 'W':
		::free(profileName);
		return Profile::Wearable;
		break;
	default:
		break;
	}
	return Profile::Invalid;
}

bool SyspopupInterface::isManufacturer(const std::string &name)
{
	bool ret = false;
	char *manufacturer = nullptr;

	::system_info_get_platform_string("tizen.org/system/manufacturer", &manufacturer);
	ret = !name.compare(manufacturer);
	::free(manufacturer);
	return ret;
}

void SyspopupInterface::createWindow()
{
	if (window == nullptr)
		window = new Window{"dpm-syspopup"};
}

void SyspopupInterface::createToastPopup()
{
	createWindow();
	switch (getProfile()) {
	case Profile::Mobile :
		popup.reset(new MobileToastPopup(window));
		break;
	case Profile::Wearable:
		popup.reset(new WearableToastPopup(window));
		break;
	default:
		throw runtime::Exception("Invalid profile");
		break;
	}
}

void SyspopupInterface::createDefaultPopup()
{
	createWindow();
	switch (getProfile()) {
	case Profile::Mobile:
		popup.reset(new MobileDefaultPopup(window));
		break;
	case Profile::Wearable:
		popup.reset(new WearableDefaultPopup(window));
		break;
	default:
		throw runtime::Exception("Invalid profile");
		break;
	}
}

void SyspopupInterface::show()
{
	if (!title.empty()) {
		popup->setTitleText(title);
	}

	if (!content.empty() && !prefix) {
		popup->setContentText(content);
	}

	if (!content.empty() && prefix) {
		popup->setPrefixContentText(content);
	}

	if (popup != nullptr) {
		popup->show();
	}
}

std::unordered_map<std::string, SyspopupInterface *> ManagedPopup::list;
