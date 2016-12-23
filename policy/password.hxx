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

#ifndef __PASSWORD_POLICY__
#define __PASSWORD_POLICY__

#include "policy-context.hxx"

namespace DevicePolicyManager {

class PasswordPolicy {
public:
	typedef enum {
		DPM_PASSWORD_QUALITY_UNSPECIFIED     = 0x00,    /**< No requirements for password. */
		DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD = 0x01,    /**< Eas requirement for simple password */
		DPM_PASSWORD_QUALITY_SOMETHING       = 0x10,    /**< Some kind password is required, but doesn't care what it is */
		DPM_PASSWORD_QUALITY_NUMERIC         = 0x20,    /**< Containing at least numeric characters */
		DPM_PASSWORD_QUALITY_ALPHABETIC      = 0x40,    /**< Containing at least alphabetic (or other symbol) characters */
		DPM_PASSWORD_QUALITY_ALPHANUMERIC    = 0x80,    /**< Containing at least numeric and alphabetic characters */
	} PasswordPolicyQuality;

	typedef enum {
		DPM_PASSWORD_STATUS_NORMAL,                  /**< Password normal status */
		DPM_PASSWORD_STATUS_CHANGED,                 /**< Password successfully changed */
		DPM_PASSWORD_STATUS_NOT_CHANGED,             /**< Password not changed */
		DPM_PASSWORD_STATUS_CHANGE_REQUIRED ,        /**< Password change required */
		DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED,   /**< Password Max Attempts Exceeded*/

		DPM_PASSWORD_STATUS_EXPIRED,					/**< Password expired */
		DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_FAILED,	/**< Device unlock failed by Password Recovery */
		DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_SUCCEEDED,/**< Device unlock succeeded by Password Recovery */

		DPM_PASSWORD_STATUS_QUALITY_CHANGED,			/**< Password quality successfully changed */
		DPM_PASSWORD_STATUS_MIN_LENGTH_CHANGED, 		/**< Password min_length successfully changed */
		DPM_PASSWORD_STATUS_COMPLEX_CHAR_CHANGED,		/**< Password complex_char successfully changed */
		DPM_PASSWORD_STATUS_PATTERN_CHANGED, 			/**< Password pattern successfully changed */
		DPM_PASSWORD_STATUS_MAX
	} PasswordPolicyStatus;

	PasswordPolicy(PolicyControlContext &ctxt);
	~PasswordPolicy();

	PasswordPolicy(const PasswordPolicy& rhs);
	PasswordPolicy(PasswordPolicy&& rhs);

	PasswordPolicy& operator=(const PasswordPolicy& rhs);
	PasswordPolicy& operator=(PasswordPolicy&& rhs);

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

private:
	struct Private;
	std::unique_ptr<Private> pimpl;
};

} /* namespace DevicePolicyManager */
#endif /* __PASSWORD_POLICY__ */
