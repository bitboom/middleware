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

#ifndef __CAPI_PASSWORD_POLICY_H__
#define __CAPI_PASSWORD_POLICY_H__

#include <dpm/device-policy-manager.h>
#include <dpm/context.h>

/**
 * @file password.h
 * @brief This file provides APIs to control password functionality
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  CAPI_DPM_PASSWORD_POLICY_MODULE
 * @{
 */

/**
 * @brief       Enumeration for dpm password quality type
 * @since_tizen 3.0
 */
typedef enum {
	DPM_PASSWORD_QUALITY_UNSPECIFIED     = 0x00,    /**< No requirements for password. */
	DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD = 0x01,    /**< EAS(Exchange ActiveSync) requirement for simple password */
	DPM_PASSWORD_QUALITY_SOMETHING       = 0x10,    /**< Some kind password is required, but doesn't care what it is */
	DPM_PASSWORD_QUALITY_NUMERIC         = 0x20,    /**< Containing at least numeric characters */
	DPM_PASSWORD_QUALITY_ALPHABETIC      = 0x40,    /**< Containing at least alphabetic (or other symbol) characters */
	DPM_PASSWORD_QUALITY_ALPHANUMERIC    = 0x80,    /**< Containing at least numeric and alphabetic characters */
} dpm_password_quality_e;

/**
 * @brief       Enumeration for dpm password status type
 * @since_tizen 3.0
 */
typedef enum {
	DPM_PASSWORD_STATUS_NORMAL,                     /**< Password normal status */
	DPM_PASSWORD_STATUS_CHANGED,                    /**< Password successfully changed */
	DPM_PASSWORD_STATUS_NOT_CHANGED,                /**< Password not changed */
	DPM_PASSWORD_STATUS_CHANGE_REQUIRED ,           /**< Password change required */
	DPM_PASSWORD_STATUS_MAX_ATTEMPTS_EXCEEDED,      /**< Password Max Attempts Exceeded*/

	DPM_PASSWORD_STATUS_EXPIRED,                    /**< Password expired */
	DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_FAILED,   /**< Device unlock failed by Password Recovery */
	DPM_PASSWORD_STATUS_RECOVERY_PASSWORD_SUCCEEDED,/**< Device unlock succeeded by Password Recovery */

	DPM_PASSWORD_STATUS_QUALITY_CHANGED,            /**< Password quality successfully changed */
	DPM_PASSWORD_STATUS_MIN_LENGTH_CHANGED,         /**< Password min_length successfully changed */
	DPM_PASSWORD_STATUS_COMPLEX_CHAR_CHANGED,       /**< Password complex_char successfully changed */
	DPM_PASSWORD_STATUS_PATTERN_CHANGED             /**< Password pattern successfully changed */
} dpm_password_status_e;

/**
 * @partner
 * @brief       Sets password quality.
 * @details     An administrator can set the password restrictions it is imposing.
 *              After setting this, the user will not be able to
 *              enter a new password that is not at least as restrictive as what has been set.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   quality Password quality type, values of #dpm_password_quality_e combined with bitwise 'or'
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_quality(device_policy_manager_h handle, int quality);

/**
 * @partner
 * @brief       Gets password quality.
 * @details     An administrator can get the password restrictions it is imposing.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   quality Password quality type, values of #dpm_password_quality_e combined with bitwise 'or'
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_quality(device_policy_manager_h handle, int *quality);

/**
 * @partner
 * @brief       Sets password minimum length.
 * @details     Sets the minimum allowed password length. After setting this,
 *              the user will not be able to enter a new password that is
 *              shorter than the setting length.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Allowed minimum password length
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_minimum_length(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets password minimum length.
 * @details     Gets the minimum allowed password length.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Allowed minimum password length
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_minimum_length(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets minimum complex char in password.
 * @details     Complex characters are all non-alphabetic characters;
 *              that is, numbers and symbols. Admin can configure this
 *              setting and make the password more secure.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Number of minimum complex char in password.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_min_complex_chars(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets minimum complex char in password.
 * @details     Complex characters are all non-alphabetic characters;
 *              that is, numbers and symbols.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Number of minimum complex char in password.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_min_complex_chars(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets maximum number of failed attempts before device is wiped.
 * @details     If user fails the last attempt, device will be wiped.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Maximum count for failed passwords.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_maximum_failed_attempts_for_wipe(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets maximum number of failed attempts before device is wiped.
 * @details     If user fails the last attempt, device will be wiped.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Maximum count for failed passwords.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_maximum_failed_attempts_for_wipe(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets the number of days password expires.
 * @details     An administrator can configure the password age to force
 *              the user to enter a new password after every expiration period.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Number of days after which the password expires.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_expires(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets the number of days password expires.
 * @details     An administrator can get the password age to force
 *              the user to enter a new password after every expiration period.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Number of days after which the password expires.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_expires(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets the number of min password history to avoid previous password.
 * @details     An administrator can configure the number of previous
 *              passwords which cannot be used when entering a new password.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Number of previous passwords which cannot be used when
 *              settings a new password.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_history(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets the number of min password history to avoid previous password.
 * @details     An administrator can get the number of previous
 *              passwords which cannot be used when entering a new password.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Number of previous passwords which cannot be used when
 *              settings a new password.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_history(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets the required password pattern.
 * @details     An administrator can force User to enter password based on
 *              a regular expression.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   pattern Password pattern. If regular expression is
 *              [a-zA-Z]{4}[0-9]{4}, we can force user to enter a 8 character
 *              password with first 4 alphabetic characters and next 4
 *              numeric characters. An administrator must take care when
 *              setting this pattern.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_pattern(device_policy_manager_h handle, const char *pattern);

/**
 * @partner
 * @brief       Resets password.
 * @details     This takes effect immediately to the device password.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   password New password
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_reset(device_policy_manager_h handle, const char *password);

/**
 * @partner
 * @brief       Enforces password change.
 * @details     An administrator can enforce password change. PasswordPolicy
 *              change setting is launched.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_enforce_change(device_policy_manager_h handle);

/**
 * @partner
 * @brief       Sets the maximum number of seconds of inactivity time
 *              before the screen timeout occurs.
 * @details     An administrator sets the maximum number of seconds of inactivity
 *              time before the screen timeout occurs and a device user must
 *              type the password to unlock the device.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Maximum inactivity time for device lock. Specifies how soon
 *              the device can be unlocked again after use, without reprompting for
 *              the passcode.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_max_inactivity_time_device_lock(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets the maximum number of seconds of inactivity time
 *              before the screen timeout occurs.
 * @details     Called by an application that is managing the device to get
 *              the value of timeout period.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]  value Pointer of Maximum inactivity time for device lock.
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_max_inactivity_time_device_lock(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets password status
 * @details     An administrator can know password status for this API.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   status Password status
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_status(device_policy_manager_h handle, dpm_password_status_e status);

/**
 * @partner
 * @brief       Gets password status
 * @details     An administrator can know password status for this API.
 * @since_tizen 3.0
 * @param[in]   handle Device policy manager handle
 * @param[out]   status Password status
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_status(device_policy_manager_h handle, dpm_password_status_e *status);


/**
 * @partner
 * @brief       Removes all password patterns.
 * @details     An administrator can remove all password patterns.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_delete_pattern(device_policy_manager_h handle);

/**
 * @partner
 * @brief       Gets password pattern.
 * @details     This API can be used for applying complexity on new password value.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @remarks     The @a pattern should be freed using free().
 * @param[in]   handle Device policy manager handle
 * @param[out]  pattern Password pattern
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_OUT_OF_MEMORY Out of memory
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_pattern(device_policy_manager_h handle, char **pattern);

/**
 * @partner
 * @brief       Sets the maximum number of times a character can occur in
 *              the device password.
 * @details     Called by an admin that is managing the device to specify that
 *              any character in the device password cannot occur more than
 *              the specified maximum number of times. Characters can be numeric
 *              or alphabetic or symbolic. "aaabcde" has 'a' which occurs 3 times,
 *              "1b1c1de" has '1' which occurs 3 times and "a@b@c@" has '@' which
 *              occurs 3 times. A value of '0' specifies that no restrictions are
 *              applied.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Maximum character occurrences
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_maximum_character_occurrences(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets the maximum number of times a character can occur in
 *              the device password.
 * @details     An administrator can retrieve the maximum number of times
 *              a character can occur in the device password. If more than
 *              one admin has set this value then the least value will take
 *              preference.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]   value Pointer of Maximum Character Occurrences
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_maximum_character_occurrences(device_policy_manager_h handle, int *value);

/**
 * @partner
 * @brief       Sets the maximum length of the numeric sequence
 *              which is allowed in the device password.
 * @details     Called by an administrator that is managing the device to set
 *              the maximum numeric sequence length. This specifies that
 *              the device password must not contain numeric sequences greater
 *              than the given length.
 *              Numeric sequences can be increasing successively by one like
 *              "12345", or decreasing successively by one like "98765", or
 *              repeating like "55555".
 *              These are all numeric sequences of length '5'.
 *              If maximum value is set to '5' then "123456" or "987654" or
 *              "555555" are not allowed.
 *              A value of '0' specifies that no such numeric sequence
 *              restrictions are applied.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   value Maximum numeric sequence length
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_maximum_numeric_sequence_length(device_policy_manager_h handle, int value);

/**
 * @partner
 * @brief       Gets the maximum numeric sequence length allowed in
 *              the device password.
 * @details     An administrator can retrieve the length of numeric sequences
 *              which are allowed in the device password.
 *              For instance, if the return value is '3' then "123", "987",
 *              "555" would all be numeric sequences of length '3' and will be
 *              allowed in the device password.
 *              If more than one admin has set this value then the least value
 *              will take preference.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[out]  value Pointer of maximum numeric sequence length
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_get_maximum_numeric_sequence_length(device_policy_manager_h handle, int *value);

/**
 * @brief       The password forbidden string list iterator handle
 * @since_tizen 3.0
 * @see         dpm_password_create_iterator()
 * @see         dpm_password_iterator_next()
 * @see         dpm_password_destroy_iterator()
 */
typedef void *dpm_password_iterator_h;

/**
 * @partner
 * @brief       Creates a password forbidden string list iterator.
 * @details     The password forbidden string list iterator can be used to get all forbidden strings.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @return      A password forbidden string list iterator on success, otherwise
 *              null value
 * @remarks     The specific error code can be obtained by using the
 *              get_last_result() method. Error codes are described in
 *              exception section.
 *              The returned iterator should be released using dpm_password_destroy_iterator().
 * @exception   #DPM_ERROR_NONE No error
 * @exception   #DPM_ERROR_OUT_OF_MEMORY Out of memory
 * @exception   #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception   #DPM_ERROR_TIMED_OUT Time out
 * @exception   #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 * @see         dpm_password_iterator_next()
 * @see         dpm_password_destroy_iterator()
 * @see         get_last_result()
 */
dpm_password_iterator_h dpm_password_create_iterator(device_policy_manager_h handle);

/**
 * @partner
 * @brief       Fetches a password forbidden string and forwards the iterator.
 * @details     This API returns a password forbidden string indicated by the iterator, and then
 *              the iterator is moved to the next position. If the iterator reaches
 *              the end of the list, null value will be returned.
 * @since_tizen 3.0
 * @param[in]   iter The iterator to be controlled
 * @param[out]  forbidden_string The forbidden string got from the iterator
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @remarks     The @a forbidden_string should not be freed using free().
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @pre         The iter must be created by dpm_password_create_iterator().
 * @see         dpm_passsword_create_iterator()
 * @see         dpm_password_destroy_iterator()
 */
int dpm_password_iterator_next(dpm_password_iterator_h iter, const char **forbidden_string);

/**
 * @partner
 * @brief       Frees the password forbidden string iterator.
 * @details     This API frees the password forbidden string iterator. This API must be called
 *              if the iterator no longer used.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   iter The iterator to be removed
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The iter must be created by dpm_password_create_iterator()
 * @see         dpm_password_create_iterator()
 * @see         dpm_password_iterator_next()
 */
int dpm_password_destroy_iterator(dpm_password_iterator_h iter);

/**
 * @partner
 * @brief       Sets strings which are forbidden in the device password.
 * @details     Called by an admin that is managing the device to set strings that are forbidden to be used in the device password.
 *              This specifies any strings which must not be present in the device password such as personal data (variations on the user's name, email address or X400 address), or any other strings.
 *              If the parameter list has only one blank string(""), then the stored strings are cleared.
 * @since_tizen 3.0
 * @privlevel   partner
 * @privilege   %http://tizen.org/privilege/dpm.password
 * @param[in]   handle Device policy manager handle
 * @param[in]   strings The forbidden strings
 * @param[in]   length The length of the strings
 * @return      #DPM_ERROR_NONE on success, otherwise a negative value
 * @retval      #DPM_ERROR_NONE Successful
 * @retval      #DPM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #DPM_ERROR_TIMED_OUT Time out
 * @retval      #DPM_ERROR_PERMISSION_DENIED The application does not have
 *              the privilege to call this API
 * @pre         The handle must be created by dpm_manager_create().
 * @see         dpm_manager_create()
 */
int dpm_password_set_forbidden_strings(device_policy_manager_h handle, const char *strings[], int length);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*  __CAPI_PASSWORD_POLICY_H__ */
