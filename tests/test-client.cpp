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
 * @file        test-client.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */

#include <klay/testbench.h>

#include <auth-passwd.h>
#include <auth-passwd-admin.h> /* for init/deinit */

#include <ctime>
#include <chrono>
#include <thread>
#include <unistd.h>

namespace {

const char *default_pass = "test-password";

struct Policy {
	unsigned int current_attempts;
	unsigned int max_attempts;
	unsigned int valid_secs;

	Policy() :
		current_attempts(0),
		max_attempts(0),
		valid_secs(0) {}
};

inline void sleep_for_retry_timeout(void)
{
	std::chrono::milliseconds dur(500);
	std::this_thread::sleep_for(dur);
}

int check_passwd(password_type type, const char *token)
{
	sleep_for_retry_timeout();

	Policy policy;

	int ret = auth_passwd_check_passwd(
			type,
			token,
			&policy.current_attempts,
			&policy.max_attempts,
			&policy.valid_secs);

	return ret;
}

int check_passwd_state(password_type type)
{
	Policy policy;

	return auth_passwd_check_passwd_state(type,
										  &policy.current_attempts,
										  &policy.max_attempts,
										  &policy.valid_secs);
}

int check_passwd_reusable(password_type type, const char *token)
{
	int is_reused = -1;
	return auth_passwd_check_passwd_reused(type, token, &is_reused);
}

int set_relative_date(int days)
{
	time_t as_is = ::time(NULL);
	time_t to_be = as_is + (days * 86400);

	return ::stime(&to_be);
}
} // anonymous namespace

TESTCASE(T0010_init)
{
	int ret = auth_passwd_reset_passwd(AUTH_PWD_NORMAL, getuid(), default_pass);

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	auth_passwd_disable_policy(getuid());
}

TESTCASE(T00111_check)
{
	int ret = check_passwd(AUTH_PWD_NORMAL, default_pass);

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00112_check_state)
{
	int ret = check_passwd_state(AUTH_PWD_NORMAL);

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00113_check_reusable)
{
	int ret = check_passwd_reusable(AUTH_PWD_NORMAL, "hoipoi");

	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00114_set)
{
	const char *new_pass = "new_pass_temp";

	sleep_for_retry_timeout();
	int ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, new_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* remove passwd */
	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, new_pass, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* rollback passwd to default pass */
	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, NULL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00115_check_passwd_available)
{
	policy_h *policy = nullptr;
	const char* not_usable_pass = "123456";
	const char* usable_pass = "tizen";

	// prepare test
	int ret = auth_passwd_new_policy(&policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_ALPHABETIC);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	sleep_for_retry_timeout();

	ret = auth_passwd_check_passwd_available(AUTH_PWD_NORMAL, not_usable_pass);
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE, ret);

	ret = auth_passwd_check_passwd_available(AUTH_PWD_NORMAL, usable_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	// restore original setting
	ret = auth_passwd_set_quality(policy, AUTH_PWD_QUALITY_UNSPECIFIED);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	auth_passwd_free_policy(policy);
}

TESTCASE(T00116_remove_password_with_empty_policy)
{
	/* precondition : set empty policy */
	policy_h *policy = nullptr;
	int ret = auth_passwd_new_policy(&policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* test : remove passwd with empty policy */
	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* restore */
	ret = auth_passwd_disable_policy(getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	auth_passwd_free_policy(policy);

	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, NULL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00117_remove_password_with_min_length_policy)
{
	/* precondition : set min-length policy */
	policy_h *policy = nullptr;
	int ret = auth_passwd_new_policy(&policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_min_length(policy, 4);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* test : remove passwd with min-length policy */
	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* restore */
	ret = auth_passwd_disable_policy(getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	auth_passwd_free_policy(policy);

	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, NULL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00118_remove_password_with_min_complex_char_policy)
{
	/* precondition : set min-complex-char policy */
	policy_h *policy = nullptr;
	int ret = auth_passwd_new_policy(&policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_min_complex_char_num(policy, AUTH_PWD_COMPLEX_CHAR_GROUP_1);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* test : remove passwd with min-complex-char policy */
	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, default_pass, NULL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* restore */
	ret = auth_passwd_disable_policy(getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
	auth_passwd_free_policy(policy);

	sleep_for_retry_timeout();
	ret = auth_passwd_set_passwd(AUTH_PWD_NORMAL, NULL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);
}

TESTCASE(T00119_check_expire_policy)
{
	/* precondition : set validity(expire time) policy */
	policy_h *policy = nullptr;
	int ret = auth_passwd_new_policy(&policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_user(policy, getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_validity(policy, 1);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = auth_passwd_set_policy(policy);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = check_passwd(AUTH_PWD_NORMAL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	ret = check_passwd_state(AUTH_PWD_NORMAL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* test : set date as +2 days */
	ret = set_relative_date(2);
	TEST_EXPECT(true, ret == 0);

	ret = check_passwd(AUTH_PWD_NORMAL, default_pass);
	TEST_EXPECT(AUTH_PASSWD_API_ERROR_PASSWORD_EXPIRED, ret);

	ret = check_passwd_state(AUTH_PWD_NORMAL);
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	/* restore */
	ret = set_relative_date(-2);
	TEST_EXPECT(true, ret == 0);

	ret = auth_passwd_disable_policy(getuid());
	TEST_EXPECT(AUTH_PASSWD_API_SUCCESS, ret);

	auth_passwd_free_policy(policy);
}
