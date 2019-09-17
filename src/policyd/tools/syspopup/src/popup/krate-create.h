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

#ifndef __DPM_KRATE_CREATE_SYSPOPUP_H__
#define __DPM_KRATE_CREATE_SYSPOPUP_H__

#include "../interface.h"

class KrateCreatePopup : public SyspopupInterface {
public:
	KrateCreatePopup();
	virtual ~KrateCreatePopup();

	void initialize();
	void contribute(const std::string &userData);
	void dispose();

private:
	AppControl setupWizard;
	Notification notification;
};

#endif /* __DPM_KRATE_CREATE_SYSPOPUP_H__ */
