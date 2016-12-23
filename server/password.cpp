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

#include <sys/types.h>

#include <unordered_map>

#include <aul.h>
#include <vconf.h>
#include <bundle.h>
#include <notification.h>
#include <system_settings.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"
#include "password-manager.h"

#include "password.hxx"

#define SIMPLE_PASSWORD_LENGTH 4

namespace DevicePolicyManager {

namespace {

std::string PasswordPattern;
std::vector<std::string> ForbiddenStrings;

std::unordered_map<uid_t, int> PasswordStatus;

inline int getPasswordPolicy(PolicyControlContext &ctx, const std::string &name)
{
	return ctx.getPolicy<int>(name, ctx.getPeerUid());
}

inline bool setPasswordPolicy(PolicyControlContext &ctx, const std::string &name, int value)
{
	return ctx.setPolicy<int>(name, value, "password", name);
}

inline PasswordManager::QualityType getPasswordQualityType(int quality)
{
	switch (quality) {
	case PasswordPolicy::DPM_PASSWORD_QUALITY_UNSPECIFIED:
		return AUTH_PWD_QUALITY_UNSPECIFIED;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD:
		return AUTH_PWD_QUALITY_UNSPECIFIED;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SOMETHING:
		return AUTH_PWD_QUALITY_SOMETHING;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_NUMERIC:
		return AUTH_PWD_QUALITY_NUMERIC;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHABETIC:
		return AUTH_PWD_QUALITY_ALPHABETIC;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHANUMERIC:
		return AUTH_PWD_QUALITY_ALPHANUMERIC;
	default:
		throw runtime::Exception("Unknown quality type: " + std::to_string(quality));
	}
}

} // namespace

struct PasswordPolicy::Private {
	Private(PolicyControlContext& ctxt);

	int setQuality(int quality);
	int getQuality();
	int setMinimumLength(int value);
	int getMinimumLength();
	int setMinComplexChars(int value);
	int getMinComplexChars();
	int setMaximumFailedForWipe(int value);
	int getMaximumFailedForWipe();
	int setExpires(int value);
	int getExpires();
	int setHistory(int value);
	int getHistory();
	int setPattern(const std::string &pattern);
	int reset(const std::string &passwd);
	int enforceChange();
	int setMaxInactivityTimeDeviceLock(int value);
	int getMaxInactivityTimeDeviceLock();
	int setStatus(int status);
	int getStatus();
	int deletePattern();
	std::string getPattern();
	int setMaximumCharacterOccurrences(int value);
	int getMaximumCharacterOccurrences();
	int setMaximumNumericSequenceLength(int value);
	int getMaximumNumericSequenceLength();
	int setForbiddenStrings(const std::vector<std::string> &forbiddenStrings);
	std::vector<std::string> getForbiddenStrings();
	int setRecovery(int enable);
	int getRecovery();

	PolicyControlContext& context;
};

PasswordPolicy::Private::Private(PolicyControlContext& ctxt) :
	context(ctxt)
{
}

int PasswordPolicy::Private::setQuality(int quality)
{
	try {
		int authQuality = DPM_PASSWORD_QUALITY_UNSPECIFIED;
		if (quality & DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
			authQuality = quality - DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD;
		}
		PasswordManager::QualityType type = getPasswordQualityType(authQuality);
		if (!setPasswordPolicy(context, "password-quality", quality)) {
			return 0;
		}

		if (quality & DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
			setPasswordPolicy(context, "password-minimum-length", SIMPLE_PASSWORD_LENGTH);
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setQuality(type);

		if (quality & DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
			passwordManager.setMinimumLength(SIMPLE_PASSWORD_LENGTH);
		}
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getQuality()
{
	return getPasswordPolicy(context, "password-quality");
}

int PasswordPolicy::Private::setMinimumLength(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-minimum-length", value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setMinimumLength(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set minimum length");
		return -1;
	}

	return 0;
}

int PasswordPolicy::Private::getMinimumLength()
{
	return getPasswordPolicy(context, "password-minimum-length");
}

int PasswordPolicy::Private::setMinComplexChars(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-minimum-complexity", value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setMinimumComplexCharacters(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set minimum complex characters");
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getMinComplexChars()
{
	return getPasswordPolicy(context, "password-minimum-complexity");
}

int PasswordPolicy::Private::setMaximumFailedForWipe(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-maximum-failure-count", (value == 0) ? INT_MAX : value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setMaximumFailedForWipe(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set maximum failed count for wipe");
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getMaximumFailedForWipe()
{
	unsigned int result = getPasswordPolicy(context, "password-maximum-failure-count");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::Private::setExpires(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-expired", (value == 0) ? INT_MAX : value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setExpires(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set expire");
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getExpires()
{
	unsigned int result = getPasswordPolicy(context, "password-expired");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::Private::setHistory(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-history", value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setHistory(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set history size");
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getHistory()
{
	return getPasswordPolicy(context, "password-history");
}

int PasswordPolicy::Private::setPattern(const std::string &pattern)
{
	try {
		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setPattern(pattern.c_str());
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR("Failed to set pattern");
		return -1;
	}

	PasswordPattern = pattern;

	return 0;
}

int PasswordPolicy::Private::reset(const std::string &passwd)
{
	try {
		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.resetPassword(passwd);
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::Private::enforceChange()
{
	int ret = 0;
	bundle *b = ::bundle_create();
	::bundle_add_str(b, "id", "password-enforce-change");

	ret = ::aul_launch_app_for_uid("org.tizen.dpm-syspopup", b, context.getPeerUid());
	::bundle_free(b);

	if (ret < 0) {
		ERROR("Failed to launch Password Application.");
		return -1;
	}

	PasswordStatus[context.getPeerUid()] = DPM_PASSWORD_STATUS_CHANGE_REQUIRED;
	return 0;
}

int PasswordPolicy::Private::setMaxInactivityTimeDeviceLock(int value)
{
	try {
		setPasswordPolicy(context, "password-inactivity-timeout", value);
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getMaxInactivityTimeDeviceLock()
{
	return getPasswordPolicy(context, "password-inactivity-timeout");
}

int PasswordPolicy::Private::setStatus(int status)
{
	if (status >= DPM_PASSWORD_STATUS_MAX) {
		return -1;
	}

	PasswordStatus[context.getPeerUid()] = status;
	context.notify("password", "password-status");

	return 0;
}

int PasswordPolicy::Private::getStatus()
{
	return PasswordStatus[context.getPeerUid()];
}

int PasswordPolicy::Private::deletePattern()
{
	try {
		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.deletePatern();
		passwordManager.enforce();
		PasswordPattern.clear();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
}

std::string PasswordPolicy::Private::getPattern()
{
	return PasswordPattern;
}

int PasswordPolicy::Private::setMaximumCharacterOccurrences(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-maximum-character-occurrences", (value == 0) ? INT_MAX : value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setMaximumCharacterOccurrences(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getMaximumCharacterOccurrences()
{
	unsigned int result = getPasswordPolicy(context, "password-maximum-character-occurrences");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::Private::setMaximumNumericSequenceLength(int value)
{
	try {
		if (!setPasswordPolicy(context, "password-numeric-sequences-length", (value == 0) ? INT_MAX : value)) {
			return 0;
		}

		PasswordManager passwordManager(context.getPeerUid());
		passwordManager.setMaximumNumericSequenceLength(value);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}
	return 0;
}

int PasswordPolicy::Private::getMaximumNumericSequenceLength()
{
	unsigned int result = getPasswordPolicy(context, "password-numeric-sequences-length");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::Private::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	try {
		PasswordManager passwordManager(context.getPeerUid());

		passwordManager.setForbiddenStrings(forbiddenStrings);
		passwordManager.enforce();
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}

	std::copy(forbiddenStrings.begin(), forbiddenStrings.end(), std::back_inserter(ForbiddenStrings));

	return 0;
}

std::vector<std::string> PasswordPolicy::Private::getForbiddenStrings()
{
	return ForbiddenStrings;
}

int PasswordPolicy::Private::setRecovery(int enable)
{
	try {
		setPasswordPolicy(context, "password-recovery", enable);
	} catch (runtime::Exception &e) {
		ERROR("Failed to set recovery");
		return -1;
	}

	return 0;
}

int PasswordPolicy::Private::getRecovery()
{
	return getPasswordPolicy(context, "password-recovery");
}

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

PasswordPolicy::PasswordPolicy(PolicyControlContext &context) :
	pimpl(new Private(context))
{
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setQuality)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMinimumLength)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMinComplexChars)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumFailedForWipe)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setExpires)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setHistory)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPattern)(std::string));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::reset)(std::string));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::enforceChange)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaxInactivityTimeDeviceLock)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setStatus)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::deletePattern)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumCharacterOccurrences)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumNumericSequenceLength)(int));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setForbiddenStrings)(std::vector<std::string>));
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setRecovery)(int));

	context.expose(this, "", (int)(PasswordPolicy::getStatus)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getQuality)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMinimumLength)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMinComplexChars)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumFailedForWipe)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getExpires)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getHistory)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaxInactivityTimeDeviceLock)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (std::string)(PasswordPolicy::getPattern)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumCharacterOccurrences)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumNumericSequenceLength)());
	context.expose(this, DPM_PRIVILEGE_PASSWORD, (std::vector<std::string>)(PasswordPolicy::getForbiddenStrings)());
	context.expose(this, "", (int)(PasswordPolicy::getRecovery)());

	context.createNotification("password");
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setQuality(int quality)
{
	return pimpl->setQuality(quality);
}

int PasswordPolicy::getQuality()
{
	return pimpl->getQuality();
}

int PasswordPolicy::setMinimumLength(int value)
{
	return pimpl->setMinimumLength(value);
}

int PasswordPolicy::getMinimumLength()
{
	return pimpl->getMinimumLength();
}

int PasswordPolicy::setMinComplexChars(int value)
{
	return pimpl->setMinComplexChars(value);
}

int PasswordPolicy::getMinComplexChars()
{
	return pimpl->getMinComplexChars();
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
{
	return pimpl->setMaximumFailedForWipe(value);
}

int PasswordPolicy::getMaximumFailedForWipe()
{
	return pimpl->getMaximumFailedForWipe();
}

int PasswordPolicy::setExpires(int value)
{
	return pimpl->setExpires(value);
}

int PasswordPolicy::getExpires()
{
	return pimpl->getExpires();
}

int PasswordPolicy::setHistory(int value)
{
	return pimpl->setHistory(value);
}

int PasswordPolicy::getHistory()
{
	return pimpl->getHistory();
}

int PasswordPolicy::setPattern(const std::string &pattern)
{
	return pimpl->setPattern(pattern);
}

int PasswordPolicy::reset(const std::string &passwd)
{
	return pimpl->reset(passwd);
}

int PasswordPolicy::enforceChange()
{
	return pimpl->enforceChange();
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	return pimpl->setMaxInactivityTimeDeviceLock(value);
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	return pimpl->getMaxInactivityTimeDeviceLock();
}

int PasswordPolicy::setStatus(int status)
{
	return pimpl->setStatus(status);
}

int PasswordPolicy::getStatus()
{
	return pimpl->getStatus();
}

int PasswordPolicy::deletePattern()
{
	return pimpl->deletePattern();
}

std::string PasswordPolicy::getPattern()
{
	return pimpl->getPattern();
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
{
	return pimpl->setMaximumCharacterOccurrences(value);
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	return pimpl->getMaximumCharacterOccurrences();
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
{
	return pimpl->setMaximumNumericSequenceLength(value);
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	return pimpl->getMaximumNumericSequenceLength();
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	return pimpl->setForbiddenStrings(forbiddenStrings);
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	return pimpl->getForbiddenStrings();
}

int PasswordPolicy::setRecovery(int enable)
{
	return pimpl->setRecovery(enable);
}

int PasswordPolicy::getRecovery()
{
	return pimpl->getRecovery();
}

DEFINE_POLICY(PasswordPolicy);

} /* namespace DevicePolicyManager*/
