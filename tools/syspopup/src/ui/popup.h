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

#ifndef __DPM_SYSPOPUP_POPUP_H__
#define __DPM_SYSPOPUP_POPUP_H__

#include <memory>

#include "widget.h"
#include "button.h"
#include "layout.h"
#include "image.h"

struct PopupInfo {
	int type;
	std::string title;
	std::string content;
};

class Popup : public Widget {
public:
	Popup(Widget *parent);
	virtual ~Popup();

	void setTimeOut(double time);
	void setAlign(double align);

	virtual void setTitleText(const std::string &text);
	virtual void setContentText(const std::string &text);
	virtual void setPrefixContentText(const std::string &text);
	virtual void setButton(const std::string &text, SmartHandler &&handler);
	virtual void setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler);

	std::function<void()> onDelete;
protected:
	Button leftButton;
	Button rightButton;
};

class MobileToastPopup : public Popup {
public:
	MobileToastPopup(Widget *parent);
	~MobileToastPopup();
};

class WearableToastPopup : public Popup {
public:
	WearableToastPopup(Widget *parent);
	~WearableToastPopup();

	void setContentText(const std::string &text);
};

class MobileDefaultPopup : public Popup {
public:
	MobileDefaultPopup(Widget *parent);
	~MobileDefaultPopup();

	void setButton(const std::string &text, SmartHandler &&handler);
	void setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler);
};

class WearableDefaultPopup : public Popup {
public:
	WearableDefaultPopup(Widget *parent);
	~WearableDefaultPopup();

	void setTitleText(const std::string &text);
	void setContentText(const std::string &text);
	void setButton(const std::string &text, SmartHandler &&handler);
	void setButton(const std::string &leftText, SmartHandler &&leftHandler, const std::string &rightText, SmartHandler &&rightHandler);

	Layout layout;

	Image leftButtonImage;
	Image rightButtonImage;
};

#endif /* __DPM_SYSPOPUP_POPUP_H__ */
