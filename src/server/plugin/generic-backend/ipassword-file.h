/*
 *  Copyright (c) 2018 Samsung Electronics Co., Ltd All Rights Reserved
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
#ifndef _IPASSWORD_FILE_H_
#define _IPASSWORD_FILE_H_

#include <string>
#include <memory>

#include <time.h>

#include <limits>

namespace AuthPasswd {

constexpr time_t PASSWORD_INFINITE_EXPIRATION_TIME = std::numeric_limits<time_t>::max();

struct IPasswordFile {
	virtual void writeMemoryToFile() const = 0;
	virtual void writeAttemptToFile() const = 0;

	virtual void setPassword(unsigned int passwdType, const std::string &password) = 0;
	virtual bool checkPassword(unsigned int passwdType,
							   const std::string &password) const = 0;

	virtual bool isPasswordActive(unsigned int passwdType) const = 0;

	virtual void setMaxHistorySize(unsigned int history) = 0;
	virtual unsigned int getMaxHistorySize() const = 0;

	virtual unsigned int getExpireTime() const = 0;
	virtual void setExpireTime(unsigned int expireTime) = 0;

	virtual unsigned int getExpireTimeLeft() const = 0;
	virtual void setExpireTimeLeft(time_t expireTimeLeft) = 0;

	virtual unsigned int getAttempt() const = 0;
	virtual void resetAttempt() = 0;
	virtual void incrementAttempt() = 0;
	virtual int getMaxAttempt() const = 0;
	virtual void setMaxAttempt(unsigned int maxAttempt) = 0;

	virtual bool isPasswordReused(const std::string &password) const = 0;

	virtual bool checkExpiration() const = 0;
	virtual bool checkIfAttemptsExceeded() const = 0;
	virtual bool isIgnorePeriod() const = 0;

	virtual bool isHistoryActive() const = 0;
};

using PasswordFileFactory = AuthPasswd::IPasswordFile* (*)(unsigned int);

} //namespace AuthPasswd

#endif
