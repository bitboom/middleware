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

#ifndef __DPM_POLICY_H__
#define __DPM_POLICY_H__

#include <string>
#include <mutex>
#include <memory>

#include <klay/xml/node.h>

class Policy {
public:
	Policy() = delete;
	Policy(xml::Node&& node);

	int getContent() const
	{
		return value;
	}

	void setContent(int content)
	{
		updateLock->lock();
		value = content;
		data.setContent(std::to_string(content));
		updateLock->unlock();
	}

private:
	int value;
	xml::Node data;
	std::unique_ptr<std::mutex> updateLock;
};

#endif //__DPM_POLICY_H__
