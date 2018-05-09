/*
 *  Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        password-file.h
 * @author      Zbigniew Jasinski (z.jasinski@samsung.com)
 * @author      Lukasz Kostyra (l.kostyra@partner.samsung.com)
 * @author      Piotr Bartosiewicz (p.bartosiewi@partner.samsung.com)
 * @author      Jooseong Lee (jooseong.lee@samsung.com)
 * @version     1.0
 * @brief       Implementation of PasswordFile, used to manage password files.
 */
#ifndef _SW_BACKEND_PASSWORD_FILE_H_
#define _SW_BACKEND_PASSWORD_FILE_H_

#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <memory>

#include <time.h>

#include <dpl/serialization.h>

#include <generic-backend/ipassword-file.h>

namespace AuthPasswd {

struct IPassword: public ISerializable {
	typedef std::vector<unsigned char> RawHash;

	enum class PasswordType : unsigned int {
		NONE = 0,
		SHA256 = 1,
	};

	virtual bool match(const std::string &password) const = 0;
};

using IPasswordPtr = std::shared_ptr<IPassword>;
using PasswordList = std::list<IPasswordPtr>;

namespace SWBackend {

class PasswordFile : public IPasswordFile {
public:
	PasswordFile(unsigned int user);
	~PasswordFile() = default;

	PasswordFile(const PasswordFile&) = delete;
	PasswordFile& operator=(const PasswordFile&) = delete;

	PasswordFile(PasswordFile&&) = delete;
	PasswordFile& operator=(PasswordFile&&) = delete;

	void writeMemoryToFile() const override;
	void writeAttemptToFile() const override;

	void setPassword(unsigned int passwdType, const std::string &password) override;
	bool checkPassword(unsigned int passwdType,
					   const std::string &password) const override;

	bool isPasswordActive(unsigned int passwdType) const override;

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

	bool isPasswordReused(const std::string &password) const override;

	bool checkExpiration() const override;
	bool checkIfAttemptsExceeded() const override;
	bool isIgnorePeriod() const override;

	bool isHistoryActive() const override;

private:
#if (__GNUC__ > 4) || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
	typedef std::chrono::steady_clock ClockType;
#else
	typedef std::chrono::monotonic_clock ClockType;
#endif
	typedef std::chrono::duration<double> TimeDiff;
	typedef std::chrono::time_point<ClockType, TimeDiff> TimePoint;

	void loadMemoryFromFile();
	bool tryLoadMemoryFromOldFormatFile();

	void resetTimer();
	void preparePwdFile();
	void prepareAttemptFile();
	void resetState();
	bool fileExists(const std::string &filename) const;
	bool dirExists(const std::string &dirpath) const;
	std::string createDir(const std::string &dir, unsigned int user) const;

	mutable TimePoint m_retryTimerStart;

	const unsigned int m_user;

	::AuthPasswd::IPasswordPtr m_passwordCurrent;
	::AuthPasswd::PasswordList m_passwordHistory;
	unsigned int m_maxAttempt;
	unsigned int m_maxHistorySize;
	unsigned int m_expireTime;
	time_t       m_expireTimeLeft;
	bool         m_passwordActive;
	bool         m_passwordRcvActive;

	//attempt file data
	unsigned int m_attempt;
};

} // namespace SWBackend
} // namespace AuthPasswd

#endif
