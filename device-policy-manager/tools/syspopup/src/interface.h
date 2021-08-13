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

#ifndef __SYSPOPUP_INTERFACE_H__
#define __SYSPOPUP_INTERFACE_H__

#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

#include <dlog.h>
#include <system_info.h>
#include <klay/exception.h>

#include "ui/window.h"
#include "ui/popup.h"
#include "ui/appcontrol.h"
#include "ui/notification.h"

class SyspopupInterface {
public:
	enum Profile {
		Mobile = 0,
		Wearable,
		Invalid,
	};

	SyspopupInterface();
	SyspopupInterface(const std::string &contentText);
	SyspopupInterface(int prefixEnabled, const std::string &contentText);
	SyspopupInterface(const std::string &titleText, const std::string &contentText);

	virtual ~SyspopupInterface();

	virtual void initialize() = 0;
	virtual void contribute(const std::string &userData) = 0;
	virtual void dispose() = 0;

	void show();

protected:
	int getProfile();
	bool isManufacturer(const std::string &name);

	void createToastPopup();
	void createDefaultPopup();

	int prefix;
	std::string title;
	std::string content;
	std::unique_ptr<Popup> popup;
private:
	void createWindow();
	Window *window;
};

class ManagedPopup {
public:
	static std::unordered_map<std::string, SyspopupInterface *> list;
};

template <typename Syspopup>
class SyspopupManager {
public:
	SyspopupManager(const std::string &id);
	~SyspopupManager();

private:
	std::unique_ptr<Syspopup> popup;
};

template <typename Syspopup>
SyspopupManager<Syspopup>::SyspopupManager(const std::string &id)
{
	popup.reset(new Syspopup{});
	ManagedPopup::list[id] = popup.get();
}

template<typename Syspopup>
SyspopupManager<Syspopup>::~SyspopupManager()
{
}

#endif /* __SYSPOPUP_INTERFACE_H__ */
