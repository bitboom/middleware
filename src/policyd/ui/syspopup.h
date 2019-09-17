/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __DPM_SYSPOPUP_SERVICE_H__
#define __DPM_SYSPOPUP_SERVICE_H__

#include <string>

#include <klay/dbus/connection.h>

class SyspopupService {
public:
	SyspopupService();
	~SyspopupService();

	void run();

private:
	dbus::Variant onMethodCall(const std::string& objectPath,
							   const std::string& interface,
							   const std::string& methodName,
							   dbus::Variant parameters);

	void onNameAcquired();
	void onNameLost();

private:
	uid_t activeUser;
};

#endif //__DPM_SYSPOPUP_SERVICE_H__
