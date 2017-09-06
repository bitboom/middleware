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
#include "widget.h"

Widget::Widget(const std::string &t, Evas_Object *obj)
	: object(obj), tag(t), disposed(false)
{
	onShow = [this]() {
	};
	onHide = [this]() {
	};
	onDispose = [this]() {
	};

	addEventHandler(EVAS_CALLBACK_SHOW, [this](void *event_info) {
		this->onShow();
	});
	addEventHandler(EVAS_CALLBACK_HIDE, [this](void *event_info) {
		this->onHide();
	});
	addEventHandler(EVAS_CALLBACK_DEL, [this](void *event_info) {
		this->onDispose();
	});

	::evas_object_size_hint_weight_set(object, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
}

Widget::~Widget()
{
	if (!disposed)
		dispose();

	eventList.clear();
	smartList.clear();
	ecoreList.clear();
}

void Widget::setText(const std::string &text)
{
	::elm_object_text_set(object, dgettext("dpm-syspopup", text.c_str()));
}

void Widget::setText(const std::string &part, const std::string &text)
{
	::elm_object_part_text_set(object, part.c_str(), dgettext("dpm-syspopup", text.c_str()));
}

void Widget::setStyle(const std::string &style)
{
	::elm_object_style_set(object, style.c_str());
}

void Widget::setContent(Widget *content)
{
	::elm_object_content_set(object, evasObject(content));
}

void Widget::setContent(const std::string &part, Widget *content)
{
	::elm_object_part_content_set(object, part.c_str(), evasObject(content));
}

void Widget::show(void)
{
	::evas_object_show(object);
}

void Widget::hide(void)
{
	::evas_object_hide(object);
}

void Widget::dispose(void)
{
	if (disposed)
		return;

	disposed = true;
	::evas_object_del(object);
}

Evas_Object *Widget::evasObject(Widget *widget)
{
	return widget ? widget->object : nullptr;
}

void Widget::smartHandlerDispatcher(void *data, Evas_Object *obj, void *event_info)
{
	SmartHandler *handler = reinterpret_cast<SmartHandler *>(data);
	(*handler)(event_info);
}

void Widget::eventHandlerDispatcher(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	EventHandler *handler = reinterpret_cast<EventHandler *>(data);
	(*handler)(event_info);
}

Eina_Bool Widget::ecoreHandlerDispatcher(void *data, int type, void *event)
{
	bool ret = EINA_FALSE;
	EcoreHandler *handler = reinterpret_cast<EcoreHandler *>(data);
	ret = (*handler)(event);
	return ret;
}

void Widget::addEventHandler(Evas_Callback_Type type, EventHandler &&handler)
{
	eventList[type] = std::move(handler);
	::evas_object_event_callback_add(object, type, eventHandlerDispatcher, reinterpret_cast<void *>(&eventList[type]));
}

void Widget::addSmartHandler(const std::string &event, SmartHandler &&handler)
{
	smartList[event] = std::move(handler);
	::evas_object_smart_callback_add(object, event.c_str(), smartHandlerDispatcher, reinterpret_cast<void *>(&smartList[event]));
}

void Widget::addEcoreHandler(int type, EcoreHandler &&handler)
{
	ecoreList[type].handler = std::move(handler);
	ecoreList[type].ecoreHandler = ::ecore_event_handler_add(type, ecoreHandlerDispatcher, reinterpret_cast<void *>(&ecoreList[type].handler));
}

void Widget::deleteEcoreHandler(int type)
{
	if (ecoreList.find(type) != ecoreList.end() && ecoreList[type].ecoreHandler != nullptr) {
		::ecore_event_handler_del(ecoreList[type].ecoreHandler);
	}
	ecoreList[type].ecoreHandler = nullptr;
	ecoreList.erase(type);
}

