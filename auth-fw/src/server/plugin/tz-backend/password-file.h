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
#ifndef _TZ_BACKEND_PASSWORD_FILE_H_
#define _TZ_BACKEND_PASSWORD_FILE_H_

#include <string>
#include <generic-backend/ipassword-file.h>

namespace AuthPasswd {
namespace TZBackend {

class PasswordFile : public IPasswordFile {
public:
	PasswordFile(unsigned int user);

	PasswordFile(const PasswordFile&) = delete;
	PasswordFile& operator=(const PasswordFile&) = delete;

	PasswordFile(PasswordFile&&) = delete;
	PasswordFile& operator=(PasswordFile&&) = delete;

	void writeMemoryToFile() const override;
	void writeAttemptToFile() const override;

	void setPassword(unsigned int passwdType, const std::string &password) override;
	bool checkPassword(unsigned int passwdType,
					   const std::string &password) override;

	void setMaxHistorySize(unsigned int history) override;
	unsigned int getMaxHistorySize() const override;

	unsigned int getExpireTime() const override;
	void setExpireTime(unsigned int expireTime) override;

	unsigned int getExpireTimeLeft() const override;
	void setExpireTimeLeft(time_t expireTimeLeft) override;

	unsigned int getAttempt() const override;
	void resetAttempt() override;
	void incrementAttempt() override;
	int getMaxAttempt() const override;
	void setMaxAttempt(unsigned int maxAttempt) override;

	bool isPasswordActive(unsigned int passwdType) const override;
	bool isPasswordReused(const std::string &password) const override;

	bool checkExpiration() const override;
	bool checkIfAttemptsExceeded() const override;
	bool isIgnorePeriod() const override;

	bool isHistoryActive() const override;
};

} // namespace TZBackend
} // namespace AuthPasswd

#endif
