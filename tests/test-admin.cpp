/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test-admin.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */

#include <klay/testbench.h>

#include "test-util.h"

#include <auth-passwd.h>
#include <auth-passwd-admin.h>

#include <unistd.h>

namespace {

const char *default_pass = "test-password";

} // anonymous namespace

TESTCASE(T00200_init)
{
	int ret = auth_passwd_reset_passwd(
			AUTH_PWD_NORMAL,
			getuid(),
			default_pass);

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00201_set_policy_params)
{
	test::ScopedPolicy sp(test::create_policy_h(), auth_passwd_free_policy);
	policy_h *policy = sp.get();
	TEST_EXPECT(true, policy != nullptr);

	int ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_max_attempts(policy, 10);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_validity(policy, 1);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_history_size(policy, 5);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_min_length(policy, 8);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_min_complex_char_num(policy, 2);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_max_char_occurrences(policy, 2);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_max_num_seq_len(policy, 2);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_SOMETHING);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_forbidden_passwd(policy, "a1b2c3d4");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

}

TESTCASE(T00202_disable_policy)
{
	int ret = auth_passwd_disable_policy(getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00203_password_qaulity)
{
	test::ScopedPolicy sp(test::create_policy_h(), auth_passwd_free_policy);
	policy_h *policy = sp.get();
	TEST_EXPECT(true, policy != nullptr);

	int ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// ===============================
	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_SOMETHING);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	ret = auth_passwd_set_policy(policy);

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "11112222");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// ===============================
	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_NUMERIC);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "!%^&*()@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "atizen12@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "aaaaaaa");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "@aaaaaaa");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// ===============================
	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_ALPHABETIC);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "1234");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "btizen12@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// ===============================
	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_ALPHANUMERIC);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "1234");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "1234@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "ctizen12@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "12dtizen@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "@12dtizen");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "12@tizen");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}


TESTCASE(T00204_min_complex_char)
{
	test::ScopedPolicy sp(test::create_policy_h(), auth_passwd_free_policy);
	policy_h *policy = sp.get();
	TEST_EXPECT(true, policy != nullptr);

	int ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_history_size(policy, 0);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_min_complex_char_num(policy, 5);
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INPUT_PARAM, ret);

	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_UNSPECIFIED);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// AUTH_PWD_COMPLEX_CHAR_GROUP_1
	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_GROUP_1);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// AUTH_PWD_COMPLEX_CHAR_GROUP_2
	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_GROUP_2);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "1234");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// AUTH_PWD_COMPLEX_CHAR_GROUP_3
	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_GROUP_3);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// AUTH_PWD_COMPLEX_CHAR_GROUP_4
	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_GROUP_4);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "tizen1@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "Tizen@");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "AAAA@1234");
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "Tizen1@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// AUTH_PWD_COMPLEX_CHAR_UNSPECIFIED
	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_UNSPECIFIED);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "1234");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "aaaaa");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, "aaaaa1234@");
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00205_pattern)
{
	const char* pattern = "[0-9]+";
	const char* allowed_pass = "Tizen123";
	const char* not_allowed_pass = "Tizen";

	test::ScopedPolicy sp(test::create_policy_h(), auth_passwd_free_policy);
	policy_h *policy = sp.get();
	TEST_EXPECT(true, policy != nullptr);

	int ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_history_size(policy, 0);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_UNSPECIFIED);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// set pattern
	ret = auth_passwd_set_pattern(policy, pattern);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, not_allowed_pass);
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_PATTERN, ret);
	sleep(1);

	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, allowed_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// restore original setting
	ret = auth_passwd_set_pattern(policy, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}
