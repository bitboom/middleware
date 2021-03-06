/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        popup.cpp
 * @author      Janusz Kozerski (j.kozerski@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "popup.h"

#include <memory>
#include <exception>

#include "common/log.h"

#define __(str) dgettext(SERVICE_NAME, str)

using namespace CCHECKER::UI;

Popup::Popup(const app_t &app) : m_app(app)
{
	m_win = elm_win_add(nullptr, "Cert Checker Popup", ELM_WIN_NOTIFICATION);
	elm_win_indicator_opacity_set(m_win, ELM_WIN_INDICATOR_TRANSLUCENT);
	elm_win_borderless_set(m_win, EINA_TRUE);
	elm_win_alpha_set(m_win, EINA_TRUE);

	m_popup = elm_popup_add(m_win);
	this->setDefaultProperties(m_popup);

	m_box = elm_box_add(m_popup);
	this->setDefaultProperties(m_box);
	evas_object_show(m_box);

	m_title = elm_label_add(m_box);
	setDefaultProperties(m_title);
	elm_object_part_text_set(m_popup, "title,text",
		__("SID_TITLE_OCSP_VERIFICATION_FAILED"));
	elm_box_pack_end(m_box, m_title);
	evas_object_show(m_title);

	m_content = elm_label_add(m_box);
	this->setDefaultProperties(m_content);

	char *buff = NULL;
	if (m_app.app_id == std::string(CCHECKER::TEMP_APP_ID)) {
		auto content = __("SID_CONTENT_OCSP_PACKAGE VERIFICATION_FAILED");
		if (-1 == asprintf(&buff, content, m_app.pkg_id.c_str()))
			throw std::bad_alloc();
	} else {
		auto content = __("SID_CONTENT_OCSP_VERIFICATION_FAILED");
		if (-1 == asprintf(&buff, content, m_app.app_id.c_str(),
				m_app.pkg_id.c_str()))
			throw std::bad_alloc();
	}
	this->setText(m_content, buff);
	free(buff);
	elm_box_pack_end(m_box, m_content);
	evas_object_show(m_content);

	elm_object_part_content_set(m_popup, "default", m_box);

	m_keepBtn = elm_button_add(m_popup);
	elm_object_style_set(m_keepBtn, "bottom");
	elm_object_text_set(m_keepBtn, __("SID_BTN_OCSP_KEEP_APP"));
	elm_object_part_content_set(m_popup, "button1", m_keepBtn);
	m_keepType = ResponseType::KEEP;
	this->callbackRegister(m_keepBtn, &m_keepType);
	evas_object_show(m_keepBtn);

	m_uninstallBtn = elm_button_add(m_popup);
	elm_object_style_set(m_uninstallBtn, "bottom");
	elm_object_text_set(m_uninstallBtn, __("SID_BTN_OCSP_UNINSTALL_APP"));
	elm_object_part_content_set(m_popup, "button2", m_uninstallBtn);
	m_uninstallType = ResponseType::UNINSTALL;
	this->callbackRegister(m_uninstallBtn, &m_uninstallType);
	evas_object_show(m_uninstallBtn);

	evas_object_show(m_popup);
	evas_object_show(m_win);
}

Popup::~Popup()
{
	evas_object_del(m_win);
}

int Popup::response = -1;

ResponseType Popup::run(void) noexcept
{
	elm_run();
	return static_cast<ResponseType>(response);
}

void Popup::setDefaultProperties(Evas_Object *obj) noexcept
{
	// Set width as maximum, height as minimum.
	evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, 0);
	// Set width as fill parent, height as center.
	evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, 0.5);
}

void Popup::setText(Evas_Object *obj, const std::string &text) noexcept
{
	// Enable text line-break automatically.
	elm_label_line_wrap_set(obj, ELM_WRAP_WORD);
	elm_object_text_set(obj, text.c_str());
}

void Popup::callbackRegister(Evas_Object *obj, ResponseType *type) noexcept
{
	evas_object_smart_callback_add(obj, "clicked", btnClickedCb, type);
}

void Popup::btnClickedCb(void *data, Evas_Object *, void *) noexcept
{
	response = *(reinterpret_cast<int *>(data));
	LogDebug("Response : " << response);
	elm_exit();
}
