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

#include "popup.h"

Popup::Popup(Widget *parent)
	: Widget("Popup", ::elm_popup_add(evasObject(parent))), leftButton(this), rightButton(this)
{
	onDispose = [this]() {
		::ui_app_exit();
	};

	addEcoreHandler(ECORE_EVENT_KEY_DOWN,
			[this](void *event) {
				Evas_Event_Key_Down *ev = (Evas_Event_Key_Down *)event;
				if (!strcmp(ev->keyname, "XF86Home")
						|| !strcmp(ev->keyname, "XF86Back")) {
					this->dispose();
				}
				return EINA_TRUE;
			});
}

Popup::~Popup()
{
}

void Popup::setTimeOut(double time)
{
	::elm_popup_timeout_set(evasObject(this), time);
	addSmartHandler("timeout", [this](void *event_info) {
		this->dispose();
	});
}

void Popup::setAlign(double align)
{
	::elm_popup_align_set(evasObject(this), ELM_NOTIFY_ALIGN_FILL, align);
}

void Popup::setTitleText(const std::string &text)
{
	setText("title,text", text.c_str());
	::elm_object_item_part_text_translatable_set(evasObject(this), "title,text", EINA_TRUE);
}

void Popup::setContentText(const std::string &text)
{
	setText(text);
}

void Popup::setPrefixContentText(const std::string &text)
{
	char *prefix = NULL;
	char *contentText = NULL;
	char prefixText[PATH_MAX] = "";

	prefix = dgettext("dpm-syspopup", "IDS_IDLE_TPOP_SECURITY_POLICY_RESTRICTS_USE_OF_PS");
	contentText = dgettext("dpm-syspopup", text.c_str());

	::snprintf(prefixText, PATH_MAX, prefix, contentText);

	setText(prefixText);
}

void Popup::setButton(const std::string &text, SmartHandler &&handler)
{
	leftButton.setText(text);
	leftButton.onClick = std::move(handler);
	setContent("button1", &leftButton);
}

void Popup::setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler)
{
	leftButton.setText(leftText);
	leftButton.onClick = std::move(leftHandler);
	setContent("button1", &leftButton);

	rightButton.setText(rightText);
	rightButton.onClick = std::move(rightHandler);
	setContent("button2", &rightButton);
}

MobileToastPopup::MobileToastPopup(Widget *parent)
	: Popup(parent)
{
	setStyle("toast");
	setTimeOut(3.0);
}

MobileToastPopup::~MobileToastPopup()
{
}

WearableToastPopup::WearableToastPopup(Widget *parent)
	: Popup(parent)
{
	setStyle("toast/circle");
	setTimeOut(3.0);
}

WearableToastPopup::~WearableToastPopup()
{
}

void WearableToastPopup::setContentText(const std::string &text)
{
	setText("elm.text", text);
}

MobileDefaultPopup::MobileDefaultPopup(Widget *parent)
	: Popup(parent)
{
	setStyle("default");
	setAlign(1.0);
}

MobileDefaultPopup::~MobileDefaultPopup()
{
}

void MobileDefaultPopup::setButton(const std::string &text, SmartHandler &&handler)
{
	Popup::setButton(text, std::forward<SmartHandler>(handler));
	leftButton.setStyle("popup");
}

void MobileDefaultPopup::setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler)
{
	Popup::setButton(leftText, std::forward<SmartHandler>(leftHandler), rightText, std::forward<SmartHandler>(rightHandler));
	leftButton.setStyle("popup");
	rightButton.setStyle("popup");
}

WearableDefaultPopup::WearableDefaultPopup(Widget *parent)
	: Popup(parent), layout(this), leftButtonImage(&leftButton), rightButtonImage(&rightButton)
{
	setStyle("circle");
	setAlign(1.0);
	setContent(&layout);
}

WearableDefaultPopup::~WearableDefaultPopup()
{
}

void WearableDefaultPopup::setTitleText(const std::string &text)
{
	layout.setText("elm.text.title", text.c_str());
}

void WearableDefaultPopup::setContentText(const std::string &text)
{
	layout.setText("elm.text", text.c_str());
}

void WearableDefaultPopup::setButton(const std::string &text, SmartHandler &&handler)
{
	layout.setTheme("layout", "popup", "content/circle/buttons1");

	leftButton.setStyle("bottom");
	leftButton.onClick = std::move(handler);

	leftButtonImage.setFile("/usr/apps/org.tizen.dpm-syspopup/res/images/tw_ic_popup_btn_delete.png");
	leftButton.setContent(&leftButtonImage);
}

void WearableDefaultPopup::setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler)
{
	layout.setTheme("layout", "popup", "content/circle/buttons2");

	leftButton.setStyle("popup/circle/left");
	leftButton.onClick = std::move(leftHandler);

	leftButtonImage.setFile("/usr/apps/org.tizen.dpm-syspopup/res/images/tw_ic_popup_btn_delete.png");
	leftButton.setContent(&leftButtonImage);

	rightButton.setStyle("popup/circle/right");
	rightButton.onClick = std::move(rightHandler);

	rightButtonImage.setFile("/usr/apps/org.tizen.dpm-syspopup/res/images/tw_ic_popup_btn_check.png");
	rightButton.setContent(&rightButtonImage);
}
