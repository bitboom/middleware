/*
 *  Authentication password
 *
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 *
 */

#ifndef AUTH_PASSWD_POLICY_TYPES_H
#define AUTH_PASSWD_POLICY_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _policy_h policy_h;

typedef enum {
	POLICY_USER,
	POLICY_MAX_ATTEMPTS,
	POLICY_VALID_PERIOD,
	POLICY_HISTORY_SIZE,
	POLICY_MIN_LENGTH,
	POLICY_MIN_COMPLEX_CHAR_NUMBER,
	POLICY_MAX_CHAR_OCCURRENCES,
	POLICY_MAX_NUMERIC_SEQ_LENGTH,
	POLICY_QUALITY_TYPE,
	POLICY_PATTERN,
	POLICY_FORBIDDEN_PASSWDS,
	POLICY_TYPE_FIRST = POLICY_MAX_ATTEMPTS,
	POLICY_TYPE_LAST = POLICY_FORBIDDEN_PASSWDS
} password_policy_type;

typedef enum {
	AUTH_PWD_NORMAL,
} password_type;

typedef enum {
	AUTH_PWD_QUALITY_UNSPECIFIED,
	AUTH_PWD_QUALITY_SOMETHING,
	AUTH_PWD_QUALITY_NUMERIC,
	AUTH_PWD_QUALITY_ALPHABETIC,
	AUTH_PWD_QUALITY_ALPHANUMERIC,
	AUTH_PWD_QUALITY_LAST = AUTH_PWD_QUALITY_ALPHANUMERIC
} password_quality_type;

typedef enum {
	AUTH_PWD_COMPLEX_CHAR_UNSPECIFIED = 0,
	AUTH_PWD_COMPLEX_CHAR_GROUP_1 = 1, // Character + Number
	AUTH_PWD_COMPLEX_CHAR_GROUP_2 = 2, // Same with GROUP1(It is the requirement from EAS.)
	AUTH_PWD_COMPLEX_CHAR_GROUP_3 = 3, // Character + Number + Special character
	AUTH_PWD_COMPLEX_CHAR_GROUP_4 = 4, // Upper case + Lower case + Number + Special character
	AUTH_PWD_COMPLEX_CHAR_LAST = AUTH_PWD_COMPLEX_CHAR_GROUP_4,
} password_complex_char_group;

#ifdef __cplusplus
}
#endif

#endif /* AUTH_PASSWD_POLICY_TYPES_H */
