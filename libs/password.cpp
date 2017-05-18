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

#include "status.h"
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

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setQuality", quality);

	return status.get();
}

int PasswordPolicy::getQuality()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getQuality");

	return status.get();
}

int PasswordPolicy::setMinimumLength(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::setMinimumLength", value);

	return status.get();
}

int PasswordPolicy::getMinimumLength()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMinimumLength");

	return status.get();
}

int PasswordPolicy::setMinComplexChars(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setMinComplexChars", value);

	return status.get();
}

int PasswordPolicy::getMinComplexChars()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMinComplexChars");

	return status.get();
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setMaximumFailedForWipe", value);

	return status.get();
}

int PasswordPolicy::getMaximumFailedForWipe()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMaximumFailedForWipe");

	return status.get();
}

int PasswordPolicy::setExpires(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setExpires", value);

	return status.get();
}

int PasswordPolicy::getExpires()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getExpires");

	return status.get();
}

int PasswordPolicy::setHistory(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setHistory", value);

	return status.get();
}

int PasswordPolicy::getHistory()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getHistory");

	return status.get();
}

int PasswordPolicy::setPattern(const std::string &pattern)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setPattern", pattern);

	return status.get();
}

int PasswordPolicy::reset(const std::string &passwd)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::reset", passwd);

	return status.get();
}

int PasswordPolicy::enforceChange()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::enforceChange");

	return status.get();
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setMaxInactivityTimeDeviceLock", value);

	return status.get();
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMaxInactivityTimeDeviceLock");;

	return status.get();
}

int PasswordPolicy::setStatus(int status)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> rstatus { -1 };

	rstatus = context.methodCall<int>("PasswordPolicy::setStatus", status);

	return rstatus.get();
}

int PasswordPolicy::getStatus()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getStatus");

	return status.get();
}

int PasswordPolicy::deletePattern()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::deletePattern");

	return status.get();
}

std::string PasswordPolicy::getPattern()
{
	PolicyControlContext& context = pimpl->context;

	Status<std::string> status { std::string() };

	status = context.methodCall<std::string>("PasswordPolicy::getPattern");

	return status.get();
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setMaximumCharacterOccurrences", value);

	return status.get();
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMaximumCharacterOccurrences");

	return status.get();
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setMaximumNumericSequenceLength", value);

	return status.get();
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { 0 };

	status = context.methodCall<int>("PasswordPolicy::getMaximumNumericSequenceLength");

	return status.get();
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setForbiddenStrings", forbiddenStrings);

	return status.get();
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	PolicyControlContext& context = pimpl->context;

	Status<std::vector<std::string>> status { std::vector<std::string>() };

	status = context.methodCall<std::vector<std::string>>("PasswordPolicy::getForbiddenStrings");

	return status.get();
}

int PasswordPolicy::setRecovery(int enable)
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { -1 };

	status = context.methodCall<int>("PasswordPolicy::setRecovery", enable);

	return status.get();
}

int PasswordPolicy::getRecovery()
{
	PolicyControlContext& context = pimpl->context;

	Status<int> status { false };

	status = context.methodCall<int>("PasswordPolicy::getRecovery");

	return status.get();
}
} /* namespace DevicePolicyManager */
