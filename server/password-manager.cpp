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

#include <klay/exception.h>

#include "password-manager.h"

PasswordManager::PasswordManager(uid_t uid) :
	user(uid)
{
	if (auth_passwd_new_policy(&p_policy) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to get auth instance");
	}

	auth_passwd_set_user(p_policy, user);
}

PasswordManager::~PasswordManager()
{
	auth_passwd_free_policy(p_policy);
}

void PasswordManager::setQuality(PasswordManager::QualityType quality)
{
	if (auth_passwd_set_quality(p_policy, quality) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to enforce password quality");
	}
}

void PasswordManager::setMinimumLength(int value)
{
	if (auth_passwd_set_min_length(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set minimum length");
	}
}

void PasswordManager::setMinimumComplexCharacters(int value)
{
	if (auth_passwd_set_min_complex_char_num(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set minimum complex characters");
	}
}

void PasswordManager::setMaximumFailedForWipe(int value)
{
	if (auth_passwd_set_max_attempts(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set maximum failed count for wipe");
	}
}

void PasswordManager::setExpires(int value)
{
	if (auth_passwd_set_validity(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set expire");
	}
}

void PasswordManager::setHistory(int value)
{
	if (auth_passwd_set_history_size(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set history size");
	}
}

void PasswordManager::setPattern(const char* pattern)
{
	if (auth_passwd_set_pattern(p_policy, pattern) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set pattern");
	}
}

void PasswordManager::deletePatern()
{
	if (auth_passwd_set_pattern(p_policy, NULL) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to delete pattern");
	}
}

void PasswordManager::setMaximumCharacterOccurrences(int value)
{
	if (auth_passwd_set_max_char_occurrences(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set maximum character occurrences");
	}
}

void PasswordManager::setMaximumNumericSequenceLength(int value)
{
	if (auth_passwd_set_max_num_seq_len(p_policy, value) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set maximum numeric sequence length");
	}
}

void PasswordManager::setForbiddenStrings(const std::vector<std::string> &forbiddenStrings)
{
	for (const std::string& str : forbiddenStrings) {
		if (auth_passwd_set_forbidden_passwd(p_policy, str.c_str()) != AUTH_PASSWD_API_SUCCESS) {
			throw runtime::Exception("Failed to set forbidden strings");
		}
	}
}

void PasswordManager::enforce()
{
	if (auth_passwd_set_policy(p_policy) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to enforce policy");
	}
}

void PasswordManager::resetPassword(const std::string& password)
{
	if (auth_passwd_reset_passwd(AUTH_PWD_NORMAL, user, password.c_str()) != AUTH_PASSWD_API_SUCCESS) {
		throw runtime::Exception("Failed to set reset password");
	}
}
