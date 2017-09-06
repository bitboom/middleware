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

#ifndef __DPM_SYSPOPUP_WIDGET_H__
#define __DPM_SYSPOPUP_WIDGET_H__

#include <functional>
#include <unordered_map>

#include <app.h>
#include <app_common.h>
#include <bundle.h>
#include <system_settings.h>
#include <Elementary.h>
#include <efl_util.h>
#include <klay/exception.h>

class Widget {
public:
	typedef std::function<void(void *event)> EventHandler;
	typedef std::function<void(void *event)> SmartHandler;
	typedef std::function<Eina_Bool(void *event)> EcoreHandler;

	virtual ~Widget();

	void setText(const std::string &text);
	void setText(const std::string &part, const std::string &text);
	void setStyle(const std::string &style);
	void setContent(Widget *content);
	void setContent(const std::string &part, Widget *content);
	void show();
	void hide();
	void dispose();

	void addEventHandler(Evas_Callback_Type type, EventHandler &&handler);
	void addSmartHandler(const std::string &event, SmartHandler &&handler);
	void addEcoreHandler(int type, EcoreHandler &&handler);
	void deleteEcoreHandler(int type);

	static void smartHandlerDispatcher(void *data, Evas_Object *obj, void *event_info);
	static void eventHandlerDispatcher(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static Eina_Bool ecoreHandlerDispatcher(void *data, int type, void *event);

	std::function<void()> onShow;
	std::function<void()> onHide;
	std::function<void()> onDispose;

protected:
	Widget(const std::string &t, Evas_Object *obj);
	virtual Evas_Object *evasObject(Widget *widget);

protected:
	std::unordered_map<int, EventHandler> eventList;
	std::unordered_map<std::string, SmartHandler> smartList;

	struct EcoreInfo {
		EcoreInfo() : ecoreHandler(nullptr) {
		}
		EcoreHandler handler;
		Ecore_Event_Handler *ecoreHandler;
	};
	std::unordered_map<int, EcoreInfo> ecoreList;
private:
	Evas_Object *object;
	std::string tag;
	bool disposed;
};

#endif /* __WIDGET_H__ */
