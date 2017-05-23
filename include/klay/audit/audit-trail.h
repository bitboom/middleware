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

#ifndef __AUDIT_TRAIL_H__
#define __AUDIT_TRAIL_H__

#include <thread>
#include <string>

#include <klay/mainloop.h>

namespace audit {

class AuditTrail {
public:
	AuditTrail();
	~AuditTrail();

	void start();
	void stop();

	void subscribe(const int fd, const runtime::Mainloop::Event events, runtime::Mainloop::Callback&& callback);
	void unsubscribe(const int fd);

private:
	runtime::Mainloop mainloop;
	std::thread dispatcher;
};

} // namespace audit
#endif //__AUDIT_TRAIL_H__
