/*
 *  Copyright (c) 2016 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Jooseong Lee <jooseong.lee@samsung.com>
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
/*
 * @file        client-password-admin.cpp
 * @author      Jooseong Lee (jooseong.lee@samsung.com)
 * @version     1.0
 * @brief       This file contains implementation of password functions.
 */

#include <cstring>

#include <dpl/log/log.h>
#include <dpl/exception.h>

#include <message-buffer.h>
#include <client-common.h>
#include <protocols.h>
#include <policy.h>

#include <auth-passwd-admin.h>

namespace {

inline bool isPasswordIncorrect(const char *pwd)
{
	// NULL means that password must be cancelled.
	return (pwd && (strlen(pwd) == 0 || strlen(pwd) > AuthPasswd::MAX_PASSWORD_LEN));
}

} // namespace anonymous

AUTH_PASSWD_API
int auth_passwd_reset_passwd(password_type passwd_type,
							 uid_t uid,
							 const char *new_passwd)
{
	using namespace AuthPasswd;
	return try_catch([&] {
		if (isPasswordIncorrect(new_passwd)) {
			LogError("Wrong input param.");
			return AUTH_PASSWD_API_ERROR_INPUT_PARAM;
		}

		if (!new_passwd)
			new_passwd = NO_PASSWORD;

		MessageBuffer send, recv;

		Serialization::Serialize(send, static_cast<int>(PasswordHdrs::HDR_RST_PASSWD));
		Serialization::Serialize(send, passwd_type);
		Serialization::Serialize(send, uid);
		Serialization::Serialize(send, std::string(new_passwd));

		int retCode = sendToServer(SERVICE_SOCKET_PASSWD_RESET, send.Pop(), recv);
		if (AUTH_PASSWD_API_SUCCESS != retCode) {
			LogError("Error in sendToServer. Error code: " << retCode);
			return retCode;
		}

		Deserialization::Deserialize(recv, retCode);

		return retCode;
	});
}

AUTH_PASSWD_API
int auth_passwd_new_policy(policy_h **pp_policy)
{
	if (!pp_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = new(std::nothrow) AuthPasswd::Policy;

	if (policy == nullptr)
		return AUTH_PASSWD_API_ERROR_OUT_OF_MEMORY;

	*pp_policy = reinterpret_cast<policy_h *>(policy);
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_user(policy_h *p_policy, uid_t uid)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_USER);
	policy->uid = uid;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_max_attempts(policy_h *p_policy, unsigned int max_attempts)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	/* TODO: set flag & value in atomic operation */
	policy->setFlag(POLICY_MAX_ATTEMPTS);
	policy->maxAttempts = max_attempts;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_validity(policy_h *p_policy, unsigned int valid_days)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_VALID_PERIOD);
	policy->validPeriod = valid_days;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_history_size(policy_h *p_policy, unsigned int history_size)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_HISTORY_SIZE);
	policy->historySize = history_size;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_min_length(policy_h *p_policy, unsigned int min_length)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_MIN_LENGTH);
	policy->minLength = min_length;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_min_complex_char_num(policy_h *p_policy, unsigned int val)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;
	if (val > AUTH_PWD_COMPLEX_CHAR_LAST)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_MIN_COMPLEX_CHAR_NUMBER);
	policy->minComplexCharNumber = val;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_max_char_occurrences(policy_h *p_policy, unsigned int val)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_MAX_CHAR_OCCURRENCES);
	policy->maxCharOccurrences = val;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_max_num_seq_len(policy_h *p_policy, unsigned int val)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_MAX_NUMERIC_SEQ_LENGTH);
	policy->maxNumSeqLength = val;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_quality(policy_h *p_policy, password_quality_type quality_type)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_QUALITY_TYPE);
	policy->qualityType = quality_type;
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_pattern(policy_h *p_policy, const char *pattern)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	if (!pattern)
		pattern = AuthPasswd::NO_PATTERN;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_PATTERN);
	policy->pattern = std::string(pattern);
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_forbidden_passwd(policy_h *p_policy, const char *forbidden_passwd)
{
	if (!p_policy)
		return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

	if (!forbidden_passwd)
		forbidden_passwd = AuthPasswd::NO_FORBIDDEND_PASSWORD;

	auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);
	policy->setFlag(POLICY_FORBIDDEN_PASSWDS);
	policy->forbiddenPasswds.insert(forbidden_passwd);
	return AUTH_PASSWD_API_SUCCESS;
}

AUTH_PASSWD_API
int auth_passwd_set_policy(policy_h *p_policy)
{
	using namespace AuthPasswd;
	return try_catch([&] {
		if (!p_policy) {
			LogError("Wrong input param.");
			return AUTH_PASSWD_API_ERROR_INPUT_PARAM;
		}

		auto policy = reinterpret_cast<AuthPasswd::Policy *>(p_policy);

		if (!policy->isFlagOn(POLICY_USER))
			return AUTH_PASSWD_API_ERROR_INPUT_PARAM;

		MessageBuffer send, recv;

		Serialization::Serialize(send, static_cast<int>(PasswordHdrs::HDR_SET_PASSWD_POLICY));

		PolicySerializable policys(*policy);
		policys.Serialize(send);

		int retCode = sendToServer(SERVICE_SOCKET_PASSWD_POLICY, send.Pop(), recv);
		if (AUTH_PASSWD_API_SUCCESS != retCode) {
			LogError("Error in sendToServer. Error code: " << retCode);
			return retCode;
		}

		Deserialization::Deserialize(recv, retCode);

		return retCode;
	});
}

AUTH_PASSWD_API
void auth_passwd_free_policy(policy_h *p_policy)
{
	delete(reinterpret_cast<AuthPasswd::Policy *>(p_policy));
}

AUTH_PASSWD_API
int auth_passwd_disable_policy(uid_t uid)
{
	using namespace AuthPasswd;
	return try_catch([&] {
		MessageBuffer send, recv;

		Serialization::Serialize(send, static_cast<int>(PasswordHdrs::HDR_DIS_PASSWD_POLICY));
		Serialization::Serialize(send, uid);

		int retCode = sendToServer(SERVICE_SOCKET_PASSWD_POLICY, send.Pop(), recv);

		if (AUTH_PASSWD_API_SUCCESS != retCode) {
			LogError("Error in sendToServer. Error code: " << retCode);
			return retCode;
		}

		Deserialization::Deserialize(recv, retCode);

		return retCode;
	});
}
