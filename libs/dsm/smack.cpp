/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "status.h"
#include "smack.hxx"

namespace DevicePolicyManager {

struct SmackSecurityMonitor::Private {
	Private(PolicyControlContext &ctx) : context(ctx) {}
	PolicyControlContext &context;
};

SmackSecurityMonitor::SmackSecurityMonitor(SmackSecurityMonitor &&rhs) = default;
SmackSecurityMonitor &SmackSecurityMonitor::operator=(SmackSecurityMonitor &&rhs) = default;

SmackSecurityMonitor::SmackSecurityMonitor(const SmackSecurityMonitor &rhs) :
	pimpl(nullptr)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

SmackSecurityMonitor &SmackSecurityMonitor::operator=(const SmackSecurityMonitor &rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

SmackSecurityMonitor::SmackSecurityMonitor(PolicyControlContext &ctx) :
	pimpl(new Private(ctx))
{
}

SmackSecurityMonitor::~SmackSecurityMonitor()
{
}

std::string SmackSecurityMonitor::getReport()
{
	PolicyControlContext &context = pimpl->context;

	Status<std::string> status { std::string() };

	status = context.methodCall<std::string>("SmackSecurityMonitor::getReport");

	return status.get();
}

int SmackSecurityMonitor::getIssueCount()
{
	PolicyControlContext &context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("SmackSecurityMonitor::getIssueCount");

	return status.get();
}

} // namespace DevicePolicyManager
