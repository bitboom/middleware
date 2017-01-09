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

#ifndef __DPM_OBSERVER_H__
#define __DPM_OBSERVER_H__
#include <sys/types.h>

#include <iostream>
#include <vector>
#include <functional>

class Observer {
public:
	virtual ~Observer() {}
	virtual void onEvent(const std::vector<uid_t>& domains) = 0;
};

class Observerable {
public:
	void attach(Observer* observer)
	{
		observers.push_back(observer);
	}

	void notify(const std::vector<uid_t>& domains)
	{
		for (auto observer: observers) {
			observer->onEvent(domains);
		}
	}

private:
	std::vector<Observer *> observers;
};

#endif //__DPM_OBSERVER_H__
