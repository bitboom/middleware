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

#ifndef __DPM_LAUNCHPAD_H__
#define __DPM_LAUNCHPAD_H__

#include <string>

#include "app-bundle.h"

class Launchpad {
public:
	Launchpad(const uid_t uid = 0);
	Launchpad(const Launchpad&) = delete;
	Launchpad(Launchpad&&) = delete;

	void launch(const std::string& appid);
	void launch(const std::string& appid, const Bundle& bundle);
	void resume(const std::string& appid);
	bool isRunning(const std::string& appid);
	void terminate(const std::string& appid);

private:
	uid_t user;
};

#endif //__DPM_LAUNCHPAD_H__
