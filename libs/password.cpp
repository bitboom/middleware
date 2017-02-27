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
#include "password.hxx"

namespace DevicePolicyManager {

struct PasswordPolicy::Private {
	Private(PolicyControlContext& ctxt) : context(ctxt) {}
	PolicyControlContext& context;
};

PasswordPolicy::PasswordPolicy(PasswordPolicy&& rhs) = default;
PasswordPolicy& PasswordPolicy::operator=(PasswordPolicy&& rhs) = default;

PasswordPolicy::PasswordPolicy(const PasswordPolicy& rhs)
{
	if (rhs.pimpl) {
		pimpl.reset(new Private(*rhs.pimpl));
	}
}

PasswordPolicy& PasswordPolicy::operator=(const PasswordPolicy& rhs)
{
	if (!rhs.pimpl) {
		pimpl.reset();
	} else {
		pimpl.reset(new Private(*rhs.pimpl));
	}

	return *this;
}

PasswordPolicy::PasswordPolicy(PolicyControlContext &ctxt) :
	pimpl(new Private(ctxt))
{
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setQuality(const int quality)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setQuality", quality);
	}
	return -1;
}

int PasswordPolicy::getQuality()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getQuality");
	}

	return 0;
}

int PasswordPolicy::setMinimumLength(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMinimumLength", value);
	}

	return 0;
}

int PasswordPolicy::getMinimumLength()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMinimumLength");
	}

	return 0;
}

int PasswordPolicy::setMinComplexChars(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMinComplexChars", value);
	}

	return -1;
}

int PasswordPolicy::getMinComplexChars()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMinComplexChars");
	}

	return 0;
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMaximumFailedForWipe", value);
	}

	return -1;
}

int PasswordPolicy::getMaximumFailedForWipe()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMaximumFailedForWipe");
	}

	return 0;
}

int PasswordPolicy::setExpires(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setExpires", value);
	}

	return -1;
}

int PasswordPolicy::getExpires()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getExpires");
	}

	return 0;
}

int PasswordPolicy::setHistory(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setHistory", value);
	}

	return -1;
}

int PasswordPolicy::getHistory()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getHistory");
	}

	return 0;
}

int PasswordPolicy::setPattern(const std::string &pattern)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setPattern", pattern);
	}

	return -1;
}

int PasswordPolicy::reset(const std::string &passwd)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::reset", passwd);
	}

	return -1;
}

int PasswordPolicy::enforceChange()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::enforceChange");
	}

	return -1;
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMaxInactivityTimeDeviceLock", value);
	}

	return -1;
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMaxInactivityTimeDeviceLock");;
	}

	return 0;
}

int PasswordPolicy::setStatus(int status)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setStatus", status);
	}

	return -1;
}

int PasswordPolicy::getStatus()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getStatus");
	}

	return 0;
}

int PasswordPolicy::deletePattern()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::deletePattern");
	}

	return -1;
}

std::string PasswordPolicy::getPattern()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<std::string>("PasswordPolicy::getPattern");
	}

	return std::string();
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMaximumCharacterOccurrences", value);
	}

	return -1;
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMaximumCharacterOccurrences");
	}

	return 0;
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setMaximumNumericSequenceLength", value);
	}

	return -1;
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getMaximumNumericSequenceLength");
	}

	return 0;
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setForbiddenStrings", forbiddenStrings);
	}

	return -1;
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<std::vector<std::string>>("PasswordPolicy::getForbiddenStrings");
	}

	return std::vector<std::string>();
}

int PasswordPolicy::setRecovery(int enable)
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::setRecovery", enable);
	}

	return -1;
}

int PasswordPolicy::getRecovery()
{
	PolicyControlContext& context = pimpl->context;
	if (context.isMaintenanceMode()) {
		return context.methodCall<int>("PasswordPolicy::getRecovery");
	}

	return false;
}
} /* namespace DevicePolicyManager */
