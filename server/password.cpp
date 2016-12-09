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

PasswordPolicy::PasswordPolicy(PolicyControlContext &ctxt) :
	context(ctxt)
{
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setQuality)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMinimumLength)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMinComplexChars)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumFailedForWipe)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setExpires)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setHistory)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPattern)(std::string));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::reset)(std::string));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::enforceChange)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaxInactivityTimeDeviceLock)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setStatus)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::deletePattern)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumCharacterOccurrences)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumNumericSequenceLength)(int));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setForbiddenStrings)(std::vector<std::string>));
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setRecovery)(int));

	ctxt.expose(this, "", (int)(PasswordPolicy::getStatus)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getQuality)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMinimumLength)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMinComplexChars)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumFailedForWipe)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getExpires)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getHistory)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaxInactivityTimeDeviceLock)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (std::string)(PasswordPolicy::getPattern)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumCharacterOccurrences)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::getMaximumNumericSequenceLength)());
	ctxt.expose(this, DPM_PRIVILEGE_PASSWORD, (std::vector<std::string>)(PasswordPolicy::getForbiddenStrings)());
	ctxt.expose(this, "", (int)(PasswordPolicy::getRecovery)());

	ctxt.createNotification("password");
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setQuality(int quality)
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

int PasswordPolicy::getQuality()
{
	return getPasswordPolicy(context, "password-quality");
}

int PasswordPolicy::setMinimumLength(int value)
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

int PasswordPolicy::getMinimumLength()
{
	return getPasswordPolicy(context, "password-minimum-length");
}

int PasswordPolicy::setMinComplexChars(int value)
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

int PasswordPolicy::getMinComplexChars()
{
	return getPasswordPolicy(context, "password-minimum-complexity");
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
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

int PasswordPolicy::getMaximumFailedForWipe()
{
	unsigned int result = getPasswordPolicy(context, "password-maximum-failure-count");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::setExpires(int value)
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

int PasswordPolicy::getExpires()
{
	unsigned int result = getPasswordPolicy(context, "password-expired");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::setHistory(int value)
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

int PasswordPolicy::getHistory()
{
	return getPasswordPolicy(context, "password-history");
}

int PasswordPolicy::setPattern(const std::string &pattern)
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

int PasswordPolicy::reset(const std::string &passwd)
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

int PasswordPolicy::enforceChange()
{
	int ret = 0;
	bundle *b = ::bundle_create();
	const char *user_data[4] = {"app-id", "org.tizen.setting-password", "caller", "DPM"};

	::bundle_add_str(b, "id", "password-enforce-change");
	::bundle_add_str_array(b, "user-data", user_data, 4);

	ret = ::aul_launch_app_for_uid("org.tizen.dpm-syspopup", b, context.getPeerUid());
	::bundle_free(b);

	if (ret < 0) {
		ERROR("Failed to launch Password Application.");
		return -1;
	}

	PasswordStatus[context.getPeerUid()] = DPM_PASSWORD_STATUS_CHANGE_REQUIRED;
	return 0;
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	try {
		setPasswordPolicy(context, "password-inactivity-timeout", value);
	} catch (runtime::Exception &e) {
		ERROR(e.what());
		return -1;
	}
	return 0;
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	return getPasswordPolicy(context, "password-inactivity-timeout");
}

int PasswordPolicy::setStatus(int status)
{
	if (status >= DPM_PASSWORD_STATUS_MAX) {
		return -1;
	}

	switch (status) {
	case DPM_PASSWORD_STATUS_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_CHANGED");
		break;
	case DPM_PASSWORD_STATUS_NOT_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_NOT_CHANGED");
		break;
	case DPM_PASSWORD_STATUS_CHANGE_REQUIRED:
		context.notify("password", "DPM_PASSWORD_STATUS_CHANGE_REQUIRED");
		break;
	case DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED:
		context.notify("password", "DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED");
		break;
	case DPM_PASSWORD_STATUS_EXPIRED:
		context.notify("password", "DPM_PASSWORD_STATUS_EXPIRED");
		break;
	case DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_FAILED:
		context.notify("password", "DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_FAILED");
		break;
	case DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_SUCCEEDED:
		context.notify("password", "DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_SUCCEEDED");
		break;
	case DPM_PASSWORD_STATUS_QUALITY_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_QUALITY_CHANGED");
		break;
	case DPM_PASSWORD_STATUS_MIN_LENGTH_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_MIN_LENGTH_CHANGED");
		break;
	case DPM_PASSWORD_STATUS_COMPLEX_CHAR_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_COMPLEX_CHAR_CHANGED");
		break;
	case DPM_PASSWORD_STATUS_PATTERN_CHANGED:
		context.notify("password", "DPM_PASSWORD_STATUS_PATTERN_CHANGED");
		break;
	}

	PasswordStatus[context.getPeerUid()] = status;

	return 0;
}

int PasswordPolicy::getStatus()
{
	return PasswordStatus[context.getPeerUid()];
}

int PasswordPolicy::deletePattern()
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

std::string PasswordPolicy::getPattern()
{
	return PasswordPattern;
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
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

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	unsigned int result = getPasswordPolicy(context, "password-maximum-character-occurrences");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
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

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	unsigned int result = getPasswordPolicy(context, "password-numeric-sequences-length");
	return (result == INT_MAX) ? 0 : result;
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
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

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	return ForbiddenStrings;
}

int PasswordPolicy::setRecovery(int enable)
{
	try {
		setPasswordPolicy(context, "password-recovery", enable);
	} catch (runtime::Exception &e) {
		ERROR("Failed to set recovery");
		return -1;
	}

	return 0;
}

int PasswordPolicy::getRecovery()
{
	return getPasswordPolicy(context, "password-recovery");
}

DEFINE_POLICY(PasswordPolicy);

} /* namespace DevicePolicyManager*/
