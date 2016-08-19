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

PasswordPolicy::PasswordPolicy(PolicyControlContext &ctxt) :
	__context(ctxt)
{
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setPasswordPolicyQuality(const int quality)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyQuality", quality);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getPasswordPolicyQuality()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getPasswordPolicyQuality");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setPasswordPolicyMinimumLength(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyMinimumLength", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getPasswordPolicyMinimumLength()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getPasswordPolicyMinimumLength");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setMinPasswordPolicyComplexChars(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setMinPasswordPolicyComplexChars", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getMinPasswordPolicyComplexChars()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getMinPasswordPolicyComplexChars");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setMaximumFailedPasswordPolicyForWipe(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setMaximumFailedPasswordPolicyForWipe", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}
int PasswordPolicy::getMaximumFailedPasswordPolicyForWipe()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getMaximumFailedPasswordPolicyForWipe");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setPasswordPolicyExpires(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyExpires", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getPasswordPolicyExpires()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getPasswordPolicyExpires");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setPasswordPolicyHistory(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyHistory", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getPasswordPolicyHistory()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getPasswordPolicyHistory");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setPasswordPolicyPattern(const std::string& pattern)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyPattern", pattern);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::resetPasswordPolicy(const std::string& passwd)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::resetPasswordPolicy", passwd);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::enforcePasswordPolicyChange()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::enforcePasswordPolicyChange");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setMaxInactivityTimeDeviceLock", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getMaxInactivityTimeDeviceLock");;
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setPasswordPolicyStatus(const int status)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setPasswordPolicyStatus", status);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getPasswordPolicyStatus()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getPasswordPolicyStatus");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::deletePasswordPolicyPattern()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::deletePasswordPolicyPattern");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

std::string PasswordPolicy::getPasswordPolicyPattern()
{
	std::string error("Error");
	try {
		return __context->methodCall<std::string>("PasswordPolicy::getPasswordPolicyPattern");
	} catch (runtime::Exception& e) {
		return error;
	}
}

int PasswordPolicy::setMaximumCharacterOccurrences(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setMaximumCharacterOccurrences", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getMaximumCharacterOccurrences");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setMaximumNumericSequenceLength(const int value)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setMaximumNumericSequenceLength", value);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	try {
		return __context->methodCall<int>("PasswordPolicy::getMaximumNumericSequenceLength");
	} catch (runtime::Exception& e) {
		return -1;
	}
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string>& forbiddenStrings)
{
	try {
		return __context->methodCall<int>("PasswordPolicy::setForbiddenStrings", forbiddenStrings);
	} catch (runtime::Exception& e) {
		return -1;
	}
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	std::vector<std::string> error;
	try {
		return __context->methodCall<std::vector<std::string>>("PasswordPolicy::getForbiddenStrings");
	} catch (runtime::Exception& e) {
		return error;
	}
}
} /* namespace DevicePolicyManager */
