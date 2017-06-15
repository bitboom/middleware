/*
 *  Authentication password
 *
 *  Copyright (c) 2000 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef AUTH_PASSWD_ERROR_H
#define AUTH_PASSWD_ERROR_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/
#define AUTH_PASSWD_API_SUCCESS 0

/*! \brief   indicating the result of the one specific API is successful */
#define AUTH_PASSWD_API_ERROR_SOCKET -1

/*! \brief   indicating the API's input parameter is malformed */
#define AUTH_PASSWD_API_ERROR_INPUT_PARAM -2

/*! \brief   indicating system  is running out of memory state */
#define AUTH_PASSWD_API_ERROR_OUT_OF_MEMORY -3

/*! \brief   indicating the output buffer size which is passed as parameter is too small */
#define AUTH_PASSWD_API_ERROR_BUFFER_TOO_SMALL -4

/*! \brief   indicating Authenticaton Server has been failed for some reason */
#define AUTH_PASSWD_API_ERROR_SERVER_ERROR -5

/*! \brief   indicating the access has been denied by Authetnication Server */
#define AUTH_PASSWD_API_ERROR_ACCESS_DENIED -6

/*! \brief   indicating there is no user */
#define AUTH_PASSWD_API_ERROR_NO_USER -8

/*! \brief   indicating there is no password set */
#define AUTH_PASSWD_API_ERROR_NO_PASSWORD -9

/*! \brief   indicating password exists in system */
#define AUTH_PASSWD_API_ERROR_PASSWORD_EXIST -11

/*! \brief   indicating password mismatch */
#define AUTH_PASSWD_API_ERROR_PASSWORD_MISMATCH -12

/*! \brief   indicating password retry timeout is not occurred yet */
#define AUTH_PASSWD_API_ERROR_PASSWORD_RETRY_TIMER -13

/*! \brief   indicating no other attempts are possible */
#define AUTH_PASSWD_API_ERROR_PASSWORD_MAX_ATTEMPTS_EXCEEDED -14

/*! \brief   indicating password is expired */
#define AUTH_PASSWD_API_ERROR_PASSWORD_EXPIRED -15

/*! \brief   indicating password is reused */
#define AUTH_PASSWD_API_ERROR_PASSWORD_REUSED -16

/*! \brief   indicating password does not meet password min length policy */
#define AUTH_PASSWD_API_ERROR_INVALID_MIN_LENGTH -18

/*! \brief   indicating password does not meet min complex character number policy */
#define AUTH_PASSWD_API_ERROR_INVALID_MIN_COMPLEX_CHAR_NUM -19

/*! \brief   indicating password does not meet max character occurences policy */
#define AUTH_PASSWD_API_ERROR_INVALID_MAX_CHAR_OCCURENCES -20

/*! \brief   indicating password does not meet max number sequence length policy */
#define AUTH_PASSWD_API_ERROR_INVALID_MAX_NUM_SEQ_LENGTH -21

/*! \brief   indicating password does not meet forbidden passwords policy */
#define AUTH_PASSWD_API_ERROR_INVALID_FORBIDDEN_PASSWORDS -22

/*! \brief   indicating password does not meet quality type policy */
#define AUTH_PASSWD_API_ERROR_INVALID_QUALITY_TYPE -23

/*! \brief   indicating password does not meet pattern policy */
#define AUTH_PASSWD_API_ERROR_INVALID_PATTERN -24

/*! \brief   indicating the error with unknown reason */
#define AUTH_PASSWD_API_ERROR_UNKNOWN -255
/** @}*/

#endif
