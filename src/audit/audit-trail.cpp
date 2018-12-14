/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <klay/audit/audit-trail.h>

namespace klay {

AuditTrail::AuditTrail()
{
}

AuditTrail::~AuditTrail()
{
	stop();
}

void AuditTrail::start()
{
	dispatcher = std::thread([this] { mainloop.run(); });
}

void AuditTrail::stop()
{
	mainloop.stop();
	if (dispatcher.joinable()) {
		dispatcher.join();
	}
}

void AuditTrail::subscribe(const int fd, const runtime::Mainloop::Event events, runtime::Mainloop::Callback&& callback)
{
	mainloop.addEventSource(fd, events, std::move(callback));
}

void AuditTrail::unsubscribe(const int fd)
{
	mainloop.removeEventSource(fd);
}

} // namespace klay
