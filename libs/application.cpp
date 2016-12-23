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
#include "application.hxx"

namespace DevicePolicyManager {

struct ApplicationPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

ApplicationPolicy::ApplicationPolicy(ApplicationPolicy&& rhs) = default;
ApplicationPolicy& ApplicationPolicy::operator=(ApplicationPolicy&& rhs) = default;

ApplicationPolicy::ApplicationPolicy(const ApplicationPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

ApplicationPolicy& ApplicationPolicy::operator=(const ApplicationPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

ApplicationPolicy::ApplicationPolicy(PolicyControlContext& ctxt) :
	pimpl(new Private(ctxt))
{
}

ApplicationPolicy::~ApplicationPolicy()
{
}

int ApplicationPolicy::installPackage(const std::string& pkgpath)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::installPackage", pkgpath);
}

int ApplicationPolicy::uninstallPackage(const std::string& pkgid)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::uninstallPackage", pkgid);
}

int ApplicationPolicy::setModeRestriction(int mode)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::setModeRestriction", mode);
}

int ApplicationPolicy::unsetModeRestriction(int mode)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::unsetModeRestriction", mode);
}

int ApplicationPolicy::getModeRestriction()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::getModeRestriction");
}

int ApplicationPolicy::addPrivilegeToBlacklist(int type, const std::string& privilege)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::addPrivilegeToBlacklist", type, privilege);
}

int ApplicationPolicy::removePrivilegeFromBlacklist(int type, const std::string& privilege)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::removePrivilegeFromBlacklist", type, privilege);
}

int ApplicationPolicy::checkPrivilegeIsBlacklisted(int type, const std::string& privilege)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("ApplicationPolicy::checkPrivilegeIsBlacklisted", type, privilege);
}
} // namespace DevicePolicyManager
