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

#include "logger.h"
#include "privilege.h"
#include "policy-builder.h"
#include "policy-model.h"

#include "password-manager.h"
#include "app-bundle.h"
#include "launchpad.h"

#include "password.hxx"

namespace DevicePolicyManager {

namespace {

const int simplePasswordLength = 4;
const int infinite = 32767;

std::unordered_map<uid_t, int> passwordStatus;

inline int canonicalize(int value)
{
	return -value;
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

class PasswordPattern {
public:
	PasswordPattern() {}
	PasswordPattern(const char *pattern_) : pattern(pattern_) {}
	PasswordPattern(const std::string& pattern_) : pattern(pattern_) {}

	bool empty() const { return pattern.empty(); }

	operator const char* () const { return pattern.c_str(); }

	bool operator<(const PasswordPattern& rhs)
	{
		pattern.append(rhs.pattern);
		return true;
	}

	bool operator>(const PasswordPattern& rhs)
	{
		pattern.append(rhs.pattern);
		return true;
	}

private:
	std::string pattern;
};

class PasswordQualityEnforceModel : public BaseEnforceModel {
public:
	PasswordQualityEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int quality, uid_t domain)
	{
		try {
			int auth = PasswordPolicy::DPM_PASSWORD_QUALITY_UNSPECIFIED;

			quality = canonicalize(quality);
			if (quality & PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
				auth = quality - PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD;
			}

			PasswordManager::QualityType type = getPasswordQualityType(auth);

			PasswordManager passwordManager(domain);
			passwordManager.setQuality(type);
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordSequencesEnforceModel : public BaseEnforceModel {
public:
	PasswordSequencesEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			value = value == infinite ? 0 : value;
			PasswordManager passwordManager(domain);
			passwordManager.setMaximumNumericSequenceLength(value);
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordOccurrencesEnforceModel : public BaseEnforceModel {
public:
	PasswordOccurrencesEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			value = value == infinite ? 0 : value;
			PasswordManager passwordManager(domain);
			passwordManager.setMaximumCharacterOccurrences(value);
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordPatternEnforceModel : public BaseEnforceModel {
public:
	PasswordPatternEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(const PasswordPattern& value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			if (value.empty()) {
				passwordManager.deletePatern();
			} else {
				passwordManager.setPattern(value);
			}
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordHistoryEnforceModel : public BaseEnforceModel {
public:
	PasswordHistoryEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			passwordManager.setHistory(canonicalize(value));
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordExpireEnforceModel : public BaseEnforceModel {
public:
	PasswordExpireEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			passwordManager.setExpires(value);
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordFailureCountEnforceModel : public BaseEnforceModel {
public:
	PasswordFailureCountEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			passwordManager.setMaximumFailedForWipe(value);
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordComplexityEnforceModel : public BaseEnforceModel {
public:
	PasswordComplexityEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			passwordManager.setMinimumComplexCharacters(canonicalize(value));
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordLengthEnforceModel : public BaseEnforceModel {
public:
	PasswordLengthEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		try {
			PasswordManager passwordManager(domain);
			passwordManager.setMinimumLength(canonicalize(value));
			passwordManager.enforce();
		} catch (runtime::Exception &e) {
			ERROR(SINK, e.what());
			return false;
		}

		return true;
	}
};

class PasswordTimeoutEnforceModel : public BaseEnforceModel {
public:
	PasswordTimeoutEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		return true;
	}
};

class PasswordRecoveryEnforceModel : public BaseEnforceModel {
public:
	PasswordRecoveryEnforceModel(PolicyControlContext& ctxt, const std::string& name) :
		BaseEnforceModel(ctxt, "password", false)
	{
	}

	bool operator()(int value, uid_t domain)
	{
		return true;
	}
};

typedef DomainPolicy<int, PasswordQualityEnforceModel> PasswordQualityPolicy;
typedef DomainPolicy<int, PasswordHistoryEnforceModel> PasswordHistoryPolicy;
typedef DomainPolicy<int, PasswordLengthEnforceModel> PasswordLengthPolicy;
typedef DomainPolicy<int, PasswordComplexityEnforceModel> PasswordComplexityPolicy;
typedef DomainPolicy<int, PasswordTimeoutEnforceModel> PasswordTimeoutPolicy;
typedef DomainPolicy<int, PasswordExpireEnforceModel> PasswordExpirePolicy;
typedef DomainPolicy<int, PasswordFailureCountEnforceModel> PasswordFailureCountPolicy;
typedef DomainPolicy<int, PasswordSequencesEnforceModel> PasswordSequencesPolicy;
typedef DomainPolicy<int, PasswordOccurrencesEnforceModel> PasswordOccurrencesPolicy;
typedef DomainPolicy<int, PasswordRecoveryEnforceModel> PasswordRecoveryPolicy;
typedef DomainPolicy<PasswordPattern, PasswordPatternEnforceModel> PasswordPatternPolicy;

struct PasswordPolicy::Private : public PolicyHelper {
	Private(PolicyControlContext& ctxt);

	PasswordQualityPolicy      quality{context, "password-quality"};
	PasswordHistoryPolicy      history{context, "password-history"};
	PasswordLengthPolicy       length{context, "password-minimum-length"};
	PasswordComplexityPolicy   complexity{context, "password-minimum-complexity"};
	PasswordTimeoutPolicy      timeout{context, "password-inactivity-timeout"};
	PasswordExpirePolicy       expire{context, "password-expired"};
	PasswordFailureCountPolicy failureCount{context, "password-maximum-failure-count"};
	PasswordSequencesPolicy    sequences{context, "password-numeric-sequences-length"};
	PasswordOccurrencesPolicy  occurrences{context, "password-maximum-character-occurrences"};
	PasswordRecoveryPolicy     recovery{context, "password-recovery"};
	PasswordPatternPolicy      pattern{context, "password-pattern"};
};

PasswordPolicy::Private::Private(PolicyControlContext& ctxt) :
	PolicyHelper(ctxt)
{
	context.createNotification("password");
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
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setQuality(int quality)
{
	try {
		pimpl->setPolicy(pimpl->quality, canonicalize(quality));
		if (quality & DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
			pimpl->setPolicy(pimpl->length, canonicalize(simplePasswordLength));
		}
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getQuality()
{
	int value = pimpl->getPolicy(pimpl->quality);
	return canonicalize(value);
}

int PasswordPolicy::setMinimumLength(int value)
{
	try {
		value = canonicalize(value);
		pimpl->setPolicy(pimpl->length, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMinimumLength()
{
	int value = pimpl->getPolicy(pimpl->length);
	return canonicalize(value);
}

int PasswordPolicy::setMinComplexChars(int value)
{
	try {
		value = canonicalize(value);
		pimpl->setPolicy(pimpl->complexity, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMinComplexChars()
{
	int value = pimpl->getPolicy(pimpl->complexity);
	return canonicalize(value);
}

int PasswordPolicy::setMaximumFailedForWipe(int value)
{
	try {
		value = value == 0 ? infinite : value;
		pimpl->setPolicy(pimpl->failureCount, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMaximumFailedForWipe()
{
	int value = pimpl->getPolicy(pimpl->failureCount);
	return value == infinite ? 0 : value;
}

int PasswordPolicy::setExpires(int value)
{
	try {
		value = value == 0 ? infinite : value;
		pimpl->setPolicy(pimpl->expire, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getExpires()
{
	int value = pimpl->getPolicy(pimpl->expire);
	return value == infinite ? 0 : value;
}

int PasswordPolicy::setHistory(int value)
{
	try {
		value = canonicalize(value);
		pimpl->setPolicy(pimpl->history, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getHistory()
{
	int value = pimpl->getPolicy(pimpl->history);
	return canonicalize(value);
}

int PasswordPolicy::setPattern(const std::string &pattern)
{
	try {
		pimpl->setPolicy(pimpl->pattern, pattern);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::reset(const std::string &passwd)
{
	try {
		PolicyControlContext& ctx = pimpl->context;
		PasswordManager passwordManager(ctx.getPeerUid());
		passwordManager.resetPassword(passwd);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::enforceChange()
{
	try {
		Bundle bundle;
		bundle.add("id", "password-enforce-change");

		PolicyControlContext& ctx = pimpl->context;

		Launchpad launchpad(ctx.getPeerUid());
		launchpad.launch("org.tizen.dpm-syspopup", bundle);
		passwordStatus[ctx.getPeerUid()] = DPM_PASSWORD_STATUS_CHANGE_REQUIRED;
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(int value)
{
	try {
		value = value == 0 ? infinite : value;
		pimpl->setPolicy(pimpl->timeout, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	int value = pimpl->getPolicy(pimpl->timeout);
	return value == infinite ? 0 : value;
}

int PasswordPolicy::setStatus(int status)
{
	if (status >= DPM_PASSWORD_STATUS_MAX) {
		ERROR(SINK, "Invalid password status");
		return -1;
	}

	PolicyControlContext& ctx = pimpl->context;
	passwordStatus[ctx.getPeerUid()] = status;
	ctx.notify("password", "password-status");

	return 0;
}

int PasswordPolicy::getStatus()
{
	PolicyControlContext& ctx = pimpl->context;
	return passwordStatus[ctx.getPeerUid()];
}

int PasswordPolicy::deletePattern()
{
	try {
		pimpl->setPolicy(pimpl->pattern, "");
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

std::string PasswordPolicy::getPattern()
{
	return std::string(pimpl->getPolicy(pimpl->pattern));
}

int PasswordPolicy::setMaximumCharacterOccurrences(int value)
{
	try {
		value = value == 0 ? infinite : value;
		pimpl->setPolicy(pimpl->occurrences, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{
	int value = pimpl->getPolicy(pimpl->occurrences);
	return value == infinite ? 0 : value;
}

int PasswordPolicy::setMaximumNumericSequenceLength(int value)
{
	try {
		value = value == 0 ? infinite : value;
		pimpl->setPolicy(pimpl->sequences, value);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	int value = pimpl->getPolicy(pimpl->sequences);
	return value == infinite ? 0 : value;
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	return 0;
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
	return std::vector<std::string>();
}

int PasswordPolicy::setRecovery(int enable)
{
	try {
		enable = canonicalize(enable);
		pimpl->setPolicy(pimpl->recovery, enable);
	} catch (runtime::Exception& e) {
		ERROR(SINK, e.what());
		return -1;
	}

	return 0;
}

int PasswordPolicy::getRecovery()
{
	int value = pimpl->getPolicy(pimpl->recovery);
	return canonicalize(value);
}

DEFINE_POLICY(PasswordPolicy);

} /* namespace DevicePolicyManager*/
