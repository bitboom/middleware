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
	return context->methodCall<int>("PasswordPolicy::setQuality", quality);
}

int PasswordPolicy::getQuality()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getQuality");
}

int PasswordPolicy::setMinimumLength(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMinimumLength", value);
}

int PasswordPolicy::getMinimumLength()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMinimumLength");
}

int PasswordPolicy::setMinComplexChars(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMinComplexChars", value);
}

int PasswordPolicy::getMinComplexChars()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMinComplexChars");
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMaximumFailedForWipe", value);
}

int PasswordPolicy::getMaximumFailedForWipe()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMaximumFailedForWipe");
}

int PasswordPolicy::setExpires(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setExpires", value);
}

int PasswordPolicy::getExpires()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getExpires");
}

int PasswordPolicy::setHistory(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setHistory", value);
}

int PasswordPolicy::getHistory()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getHistory");
}

int PasswordPolicy::setPattern(const std::string &pattern)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setPattern", pattern);
}

int PasswordPolicy::reset(const std::string &passwd)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::reset", passwd);
}

int PasswordPolicy::enforceChange()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::enforceChange");
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMaxInactivityTimeDeviceLock", value);
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMaxInactivityTimeDeviceLock");;
}

int PasswordPolicy::setStatus(int status)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setStatus", status);
}

int PasswordPolicy::getStatus()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getStatus");
}

int PasswordPolicy::deletePattern()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::deletePattern");
}

std::string PasswordPolicy::getPattern()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<std::string>("PasswordPolicy::getPattern");
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMaximumCharacterOccurrences", value);
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMaximumCharacterOccurrences");
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setMaximumNumericSequenceLength", value);
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getMaximumNumericSequenceLength");
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setForbiddenStrings", forbiddenStrings);
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<std::vector<std::string>>("PasswordPolicy::getForbiddenStrings");
}

int PasswordPolicy::setRecovery(int enable)
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::setRecovery", enable);
}

int PasswordPolicy::getRecovery()
{
	PolicyControlContext& context = pimpl->context;
	return context->methodCall<int>("PasswordPolicy::getRecovery");
}
} /* namespace DevicePolicyManager */
