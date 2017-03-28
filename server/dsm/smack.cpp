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
#include <klay/exception.h>
#include <klay/audit/logger.h>

#include "policy-builder.h"
#include "client-manager.h"

#include "smack.hxx"
#include "smack_report_issuer.h"

namespace DevicePolicyManager {

struct SmackSecurityMonitor::Private {
	Private(PolicyControlContext &ctx) : context(ctx) {}
	PolicyControlContext &context;
};

SmackSecurityMonitor::SmackSecurityMonitor(SmackSecurityMonitor &&rhs) = default;
SmackSecurityMonitor &SmackSecurityMonitor::operator=(SmackSecurityMonitor &&rhs) = default;

SmackSecurityMonitor::SmackSecurityMonitor(const SmackSecurityMonitor &rhs)
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

SmackSecurityMonitor::SmackSecurityMonitor(PolicyControlContext &context) :
	pimpl(new Private(context))
{
	context.expose(this, "", (std::string)(SmackSecurityMonitor::getReport)());
}

SmackSecurityMonitor::~SmackSecurityMonitor()
{
}

std::string SmackSecurityMonitor::getReport()
{
	SmackReportIssuer issuer;

	return issuer.generateReport();
}

DEFINE_POLICY(SmackSecurityMonitor);

} // namespace DevicePolicyManager
