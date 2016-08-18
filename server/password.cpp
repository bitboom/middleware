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

#include <aul.h>
#include <vconf.h>
#include <bundle.h>
#include <notification.h>
#include <system_settings.h>
#include <auth-passwd-admin.h>
#include <klay/auth/user.h>
#include <klay/audit/logger.h>

#include "privilege.h"
#include "policy-builder.h"

#include "password.hxx"

#define SIMPLE_PASSWORD_LENGTH 4
#define PASSWORD_EMPTY_STRING "\\n"

namespace DevicePolicyManager {

namespace {

PasswordPolicy::PasswordPolicyQuality qualityType;
std::string PasswordPattern;
std::vector<std::string> ForbiddenStrings;

inline int getPasswordPolicy(PolicyControlContext& ctx, const std::string& name)
{
	return ctx.getPolicy(name, ctx.getPeerUid());
}

inline int setPasswordPolicy(PolicyControlContext& ctx, const std::string& name, int value)
{
	return ctx.setPolicy(name, value, "password", name);
}

int createNotificationLaunch(void)
{
	int lock_type = 0, view_type = 0, ret;
	app_control_h app_control = NULL;
	static notification_h passwdNoti = NULL;
	char sViewtype[][40] = {"SETTING_PW_TYPE_SET_SIMPLE_PASSWORD", "SETTING_PW_TYPE_SET_PASSWORD"};

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);
	if (lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD)
		view_type = 0;
	else
		view_type = 1;

	passwdNoti = notification_create(NOTIFICATION_TYPE_NOTI);
	if (passwdNoti == NULL) {
		return -1;
	}

	ret = app_control_create(&app_control);
	if (ret != APP_CONTROL_ERROR_NONE) {
		notification_free(passwdNoti);
		return -1;
	}

	ret = 0;
	try {
		ret = notification_set_text(passwdNoti, NOTIFICATION_TEXT_TYPE_TITLE, "Change password", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if (ret != NOTIFICATION_ERROR_NONE) {
			throw runtime::Exception("notification_set_text1 error");
		}

		ret = notification_set_text(passwdNoti, NOTIFICATION_TEXT_TYPE_CONTENT, "Tap here to change password", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if (ret != NOTIFICATION_ERROR_NONE) {
			throw runtime::Exception("notification_set_text2 error");
		}

		ret = notification_set_display_applist(passwdNoti, NOTIFICATION_DISPLAY_APP_ALL);
		if (ret != NOTIFICATION_ERROR_NONE) {
			throw runtime::Exception("notification_set_display_applist error");
		}

		ret = app_control_set_app_id(app_control, "setting-password-efl");
		if (ret != APP_CONTROL_ERROR_NONE) {
			throw runtime::Exception("app_control_set_app_id error");
		}

		ret = app_control_add_extra_data(app_control, "viewtype", sViewtype[view_type]);
		if (ret != APP_CONTROL_ERROR_NONE) {
			throw runtime::Exception("app_control_add_extra_data1 error");
		}

		ret = app_control_add_extra_data(app_control, "caller", "DPM");
		if (ret != APP_CONTROL_ERROR_NONE) {
			throw runtime::Exception("app_control_add_extra_data2 error");
		}

		ret = notification_set_launch_option(passwdNoti, NOTIFICATION_LAUNCH_OPTION_APP_CONTROL, (void *)app_control);
		if (ret != NOTIFICATION_ERROR_NONE) {
			throw runtime::Exception("notification_set_launch_option error");
		}

		ret = notification_post(passwdNoti);
		if (ret != NOTIFICATION_ERROR_NONE) {
			throw runtime::Exception("notification_post error");
		}
	} catch (runtime::Exception e) {
		ERROR(e.what());
		ret = -1;
	}

	app_control_destroy(app_control);
	notification_free(passwdNoti);

	return ret;
}

int transformValueFromIntToQualityType(const int quality, PasswordPolicy::PasswordPolicyQuality &changed_quality)
{
	switch (quality) {
	case PasswordPolicy::DPM_PASSWORD_QUALITY_UNSPECIFIED:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_UNSPECIFIED;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SOMETHING:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_SOMETHING;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_NUMERIC:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_NUMERIC;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHABETIC:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHABETIC;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHANUMERIC:
		changed_quality = PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHANUMERIC;
		break;
	default:
		ERROR("unknown password quality");
		return -1;
		break;
	}

	return 0;
}

int transformQualityFromDPMToAuth(const int dpm_quality, password_quality_type &auth_quality)
{
	switch (dpm_quality) {
	case PasswordPolicy::DPM_PASSWORD_QUALITY_UNSPECIFIED:
		auth_quality = AUTH_PWD_QUALITY_UNSPECIFIED;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD:
		auth_quality = AUTH_PWD_QUALITY_UNSPECIFIED;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_SOMETHING:
		auth_quality = AUTH_PWD_QUALITY_SOMETHING;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_NUMERIC:
		auth_quality = AUTH_PWD_QUALITY_NUMERIC;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHABETIC:
		auth_quality = AUTH_PWD_QUALITY_ALPHABETIC;
		break;
	case PasswordPolicy::DPM_PASSWORD_QUALITY_ALPHANUMERIC:
		auth_quality = AUTH_PWD_QUALITY_ALPHANUMERIC;
		break;
	default:
		ERROR("unknown");
		return -1;
		break;
	}

	return 0;
}

} // namespace

PasswordPolicy::PasswordPolicy(PolicyControlContext &ctxt) :
	context(ctxt)
{
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyQuality)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyMinimumLength)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMinPasswordPolicyComplexChars)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumFailedPasswordPolicyForWipe)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyExpires)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyHistory)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyPattern)(std::string));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::resetPasswordPolicy)(std::string));
	ctxt.registerNonparametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::enforcePasswordPolicyChange));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaxInactivityTimeDeviceLock)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setPasswordPolicyStatus)(int));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getPasswordPolicyStatus));
	ctxt.registerNonparametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::deletePasswordPolicyPattern));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumCharacterOccurrences)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setMaximumNumericSequenceLength)(int));
	ctxt.registerParametricMethod(this, DPM_PRIVILEGE_PASSWORD, (int)(PasswordPolicy::setForbiddenStrings)(std::vector<std::string>));

	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getPasswordPolicyQuality));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getPasswordPolicyMinimumLength));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getMinPasswordPolicyComplexChars));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getMaximumFailedPasswordPolicyForWipe));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getPasswordPolicyExpires)());
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getPasswordPolicyHistory)());
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getMaxInactivityTimeDeviceLock));
	ctxt.registerNonparametricMethod(this, "", (std::string)(PasswordPolicy::getPasswordPolicyPattern));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getMaximumCharacterOccurrences));
	ctxt.registerNonparametricMethod(this, "", (int)(PasswordPolicy::getMaximumNumericSequenceLength));
	ctxt.registerNonparametricMethod(this, "", (std::vector<std::string>)(PasswordPolicy::getForbiddenStrings));

	ctxt.createNotification("password");
}

PasswordPolicy::~PasswordPolicy()
{
}

int PasswordPolicy::setPasswordPolicyQuality(const int quality)
{
	policy_h *p_policy;
	password_quality_type auth_quality_type = AUTH_PWD_QUALITY_UNSPECIFIED;

	if (transformValueFromIntToQualityType(quality, qualityType) != 0) {
		return -1;
	}

	if (transformQualityFromDPMToAuth(qualityType, auth_quality_type) != 0) {
		return -1;
	}

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_quality(p_policy, auth_quality_type) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (qualityType == PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
		if (auth_passwd_set_min_length(p_policy, SIMPLE_PASSWORD_LENGTH) != AUTH_PASSWD_API_SUCCESS) {
			auth_passwd_free_policy(p_policy);
			return -1;
		}
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	setPasswordPolicy(context, "password-quality", quality);
	if (qualityType == PasswordPolicy::DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD) {
		setPasswordPolicy(context, "password-minimum-length", SIMPLE_PASSWORD_LENGTH);
	}

	return 0;
}

int PasswordPolicy::getPasswordPolicyQuality()
{
	return getPasswordPolicy(context, "password-quality");
}

int PasswordPolicy::setPasswordPolicyMinimumLength(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_min_length(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-minimum-length", value);
}

int PasswordPolicy::getPasswordPolicyMinimumLength()
{
	return getPasswordPolicy(context, "password-minimum-length");
}

int PasswordPolicy::setMinPasswordPolicyComplexChars(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_min_complex_char_num(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-minimum-complexity", value);
}

int PasswordPolicy::getMinPasswordPolicyComplexChars()
{
	return getPasswordPolicy(context, "password-minimum-complexity");
}

int PasswordPolicy::setMaximumFailedPasswordPolicyForWipe(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_max_attempts(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-maximum-failure-count", (value == 0) ? UINT_MAX : value);
}

int PasswordPolicy::getMaximumFailedPasswordPolicyForWipe()
{
	unsigned int result = getPasswordPolicy(context, "password-maximum-failure-count");
	return (result == UINT_MAX)? 0 : result;
}

int PasswordPolicy::setPasswordPolicyExpires(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_validity(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-expired", (value == 0) ? UINT_MAX : value);
}

int PasswordPolicy::getPasswordPolicyExpires()
{
	unsigned int result = getPasswordPolicy(context, "password-expired");
	return (result == UINT_MAX)? 0 : result;
}

int PasswordPolicy::setPasswordPolicyHistory(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_history_size(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-history", value);
}

int PasswordPolicy::getPasswordPolicyHistory()
{
	return getPasswordPolicy(context, "password-history");
}

int PasswordPolicy::setPasswordPolicyPattern(const std::string &pattern)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_pattern(p_policy, pattern.c_str()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	PasswordPattern = pattern;
	if (PasswordPattern.compare("") == 0)
		PasswordPattern = PASSWORD_EMPTY_STRING;

	return 0;//SetPolicy(context, "password-pattern", PasswordPattern);
}

int PasswordPolicy::resetPasswordPolicy(const std::string &passwd)
{
	int ret = 0;

	if (auth_passwd_reset_passwd(AUTH_PWD_NORMAL, context.getPeerUid(), passwd.c_str()) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	return ret;
}

int PasswordPolicy::enforcePasswordPolicyChange()
{
	int ret = 0;
	int lock_type = 0;
	bundle *b = ::bundle_create();
	const char *simple_user_data[6] = {"app-id", "setting-password-efl", "caller", "DPM", "viewtype", "SETTING_PW_TYPE_SET_SIMPLE_PASSWORD"};
	const char *passwd_user_data[6] = {"app-id", "setting-password-efl", "caller", "DPM", "viewtype", "SETTING_PW_TYPE_SET_PASSWORD"};

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);
	::bundle_add_str(b, "id", "password-enforce-change");
	if (lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD)
		::bundle_add_str_array(b, "user-data", simple_user_data, 6);
	else
		::bundle_add_str_array(b, "user-data", passwd_user_data, 6);
	ret = ::aul_launch_app_for_uid("org.tizen.dpm-syspopup", b, context.getPeerUid());
	::bundle_free(b);

	if (ret < 0) {
		ERROR("Failed to launch Password Application.");
		return -1;
	} else {
		return setPasswordPolicy(context, "password-status", PasswordPolicy::DPM_PASSWORD_STATUS_CHANGE_REQUIRED);
	}
}

int PasswordPolicy::setMaxInactivityTimeDeviceLock(const int value)
{
	return setPasswordPolicy(context, "password-inactivity-timeout", value);
}

int PasswordPolicy::getMaxInactivityTimeDeviceLock()
{
	return getPasswordPolicy(context, "password-inactivity-timeout");
}

int PasswordPolicy::setPasswordPolicyStatus(const int status)
{
	int current_status = getPasswordPolicy(context, "password-status");

	if (status >= PasswordPolicy::DPM_PASSWORD_STATUS_MAX) {
		return -1;
	}

	if (status == PasswordPolicy::DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED) {
		ERROR("Max Attempts Exceeded.");
		return -1;
	}

	if (current_status == PasswordPolicy::DPM_PASSWORD_STATUS_CHANGE_REQUIRED) {
		if (status == PasswordPolicy::DPM_PASSWORD_STATUS_CHANGED) {
			return setPasswordPolicy(context, "password-status", PasswordPolicy::DPM_PASSWORD_STATUS_NORMAL);
		} else if (status == PasswordPolicy::DPM_PASSWORD_STATUS_NOT_CHANGED) {
			return createNotificationLaunch();
		}
	} else if (current_status ==  PasswordPolicy::DPM_PASSWORD_STATUS_NORMAL) {
		if (status == PasswordPolicy::DPM_PASSWORD_STATUS_CHANGE_REQUIRED) {
			return setPasswordPolicy(context, "password-status", status);
		}
	}

	return -1;
}

int PasswordPolicy::getPasswordPolicyStatus()
{
	int status = context.getPolicy("password-status");
	return status;
}

int PasswordPolicy::deletePasswordPolicyPattern()
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_pattern(p_policy, NULL) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	PasswordPattern.clear();
	return 0;//SetPolicy(context, "password-pattern", PASSWORD_EMPTY_STRING);
}

std::string PasswordPolicy::getPasswordPolicyPattern()
{
	PasswordPattern = getPasswordPolicy(context, "password-pattern");
	if (PasswordPattern.compare(PASSWORD_EMPTY_STRING) == 0)
		PasswordPattern = "";

	return PasswordPattern;
}

int PasswordPolicy::setMaximumCharacterOccurrences(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_max_char_occurrences(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-maximum-character-occurrences", (value == 0) ? UINT_MAX : value);
}

int PasswordPolicy::getMaximumCharacterOccurrences()
{

    unsigned int result = getPasswordPolicy(context, "password-maximum-character-occurrences");
	return (result == UINT_MAX)? 0 : result;
}

int PasswordPolicy::setMaximumNumericSequenceLength(const int value)
{
	policy_h *p_policy;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_max_num_seq_len(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	return setPasswordPolicy(context, "password-numeric-sequences-length", (value == 0) ? UINT_MAX : value);
}

int PasswordPolicy::getMaximumNumericSequenceLength()
{
	unsigned int result = getPasswordPolicy(context, "password-numeric-sequences-length");
	return (result == UINT_MAX)? 0 : result;
}

int PasswordPolicy::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	unsigned int count = 0;
	policy_h *p_policy;
	std::string xmlForbiddenStrings;

	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		return -1;
	}

	if (auth_passwd_set_user(p_policy, context.getPeerUid()) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	for (std::vector<std::string>::const_iterator iter = forbiddenStrings.begin(); iter != forbiddenStrings.end(); ++iter) {
		if (auth_passwd_set_forbidden_passwd(p_policy, (*iter).c_str()) != AUTH_PASSWD_API_SUCCESS) {
			auth_passwd_free_policy(p_policy);
			return -1;
		}
	}

	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		auth_passwd_free_policy(p_policy);
		return -1;
	}

	auth_passwd_free_policy(p_policy);

	std::copy(forbiddenStrings.begin(), forbiddenStrings.end(), std::back_inserter(ForbiddenStrings));
	for (std::vector<std::string>::const_iterator iter = forbiddenStrings.begin(); iter != forbiddenStrings.end(); ++iter) {
		xmlForbiddenStrings += (*iter).c_str();
		if (++count < forbiddenStrings.size())
			xmlForbiddenStrings += PASSWORD_EMPTY_STRING;
	}

	if (xmlForbiddenStrings.compare("") == 0) {
		xmlForbiddenStrings = PASSWORD_EMPTY_STRING;
	}

	return 0;//SetPolicy(context, "password-forbidden-strings", xmlForbiddenStrings.c_str());
}

std::vector<std::string> PasswordPolicy::getForbiddenStrings()
{
#if 0
	unsigned int nPos;
	std::string xmlForbiddenStrings;

	xmlForbiddenStrings = getPasswordPolicy(context, "password-forbidden-strings");
	ForbiddenStrings.clear();
	while ((nPos = xmlForbiddenStrings.find_first_of(PASSWORD_EMPTY_STRING)) != xmlForbiddenStrings.npos) {
		if (nPos > 0) {
			ForbiddenStrings.push_back(xmlForbiddenStrings.substr(0, nPos));
		}
		xmlForbiddenStrings = xmlForbiddenStrings.substr(nPos + 3);
	}
	if (xmlForbiddenStrings.length() > 0)
		ForbiddenStrings.push_back(xmlForbiddenStrings.substr(0, nPos));

	if ((*ForbiddenStrings.begin()).compare(PASSWORD_EMPTY_STRING) == 0)
		*ForbiddenStrings.begin() = "";
#endif

	return ForbiddenStrings;
}

DEFINE_POLICY(PasswordPolicy);

} /* namespace DevicePolicyManager*/
