/*
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
 */
/*
 * @file        password-file.cpp
 * @author      Zbigniew Jasinski (z.jasinski@samsung.com)
 * @author      Lukasz Kostyra (l.kostyra@partner.samsung.com)
 * @author      Piotr Bartosiewicz (p.bartosiewi@partner.samsung.com)
 * @author      Jooseong Lee (jooseong.lee@samsung.com)
 * @version     1.0
 * @brief       Implementation of PasswordFile, used to manage password files.
 */
#include <sw-backend/password-file.h>

#include <fstream>
#include <algorithm>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dpl/log/log.h>
#include <dpl/fstream_accessors.h>

#include <auth-passwd-policy-types.h>
#include <auth-passwd-error.h>

#include <error-description.h>
#include <policy.h>
#include <password-exception.h>
#include <generic-backend/password-file-buffer.h>

extern "C" {

AuthPasswd::IPasswordFile* PasswordFileFactory(unsigned int user)
{
	return new AuthPasswd::SWBackend::PasswordFile(user);
}

} // extern "C"

namespace {
const std::string PASSWORD_FILE = "/password";
const std::string OLD_VERSION_PASSWORD_FILE = "/password.old";
const std::string ATTEMPT_FILE = "/attempt";
const double RETRY_TIMEOUT = 0.5;
const mode_t FILE_MODE = S_IRUSR | S_IWUSR;
const unsigned int CURRENT_FILE_VERSION = 4;
} // namespace anonymous

namespace AuthPasswd {
namespace SWBackend {

PasswordFile::PasswordFile(unsigned int user) :
	m_user(user),
	m_passwordCurrent(new NoPassword()),
	m_maxAttempt(PASSWORD_INFINITE_ATTEMPT_COUNT),
	m_maxHistorySize(0),
	m_expireTime(PASSWORD_INFINITE_EXPIRATION_DAYS),
	m_expireTimeLeft(PASSWORD_INFINITE_EXPIRATION_TIME),
	m_passwordActive(false),
	m_passwordRcvActive(false),
	m_attempt(0)
{
	// check if data directory exists
	// if not create it
	std::string userDir = createDir(RW_DATA_DIR, m_user);

	if (!dirExists(RW_DATA_DIR)) {
		if (mkdir(RW_DATA_DIR, 0700)) {
			LogError("Failed to create directory for files. Error: " << errnoToString());
			Throw(PasswordException::MakeDirError);
		}
	}

	if (!dirExists(userDir.c_str())) {
		if (mkdir(userDir.c_str(), 0700)) {
			LogError("Failed to create directory for files. Error: " << errnoToString());
			Throw(PasswordException::MakeDirError);
		}
	}

	preparePwdFile();
	prepareAttemptFile();
	resetTimer();
}

void PasswordFile::resetState()
{
	m_maxAttempt = PASSWORD_INFINITE_ATTEMPT_COUNT;
	m_maxHistorySize = 0;
	m_expireTime = PASSWORD_INFINITE_EXPIRATION_DAYS;
	m_expireTimeLeft = PASSWORD_INFINITE_EXPIRATION_TIME;
	m_passwordRcvActive = false;
	m_passwordActive = false;
	m_passwordCurrent.reset(new NoPassword());
}

void PasswordFile::resetTimer()
{
	m_retryTimerStart = ClockType::now();
	m_retryTimerStart -= TimeDiff(RETRY_TIMEOUT);
}

void PasswordFile::preparePwdFile()
{
	std::string pwdFile = createDir(RW_DATA_DIR, m_user) + PASSWORD_FILE;
	std::string oldVersionPwdFile = createDir(RW_DATA_DIR, m_user) + OLD_VERSION_PASSWORD_FILE;

	// check if password file exists
	if (!fileExists(pwdFile)) {
		// if old format file exist - load it
		if (tryLoadMemoryFromOldFormatFile()) {
			// save in new format
			writeMemoryToFile();

			// and remove old file
			if (remove(oldVersionPwdFile.c_str())) {
				LogError("Failed to remove file" << oldVersionPwdFile <<
						 " Error: " << errnoToString());
				Throw(PasswordException::RemoveError);
			}

			return;
		}

		LogSecureDebug("PWD_DBG not found " << m_user << " password file. Creating.");
		//create file
		writeMemoryToFile();
	} else {     //if file exists, load data
		LogSecureDebug("PWD_DBG found " << m_user << " password file. Opening.");

		try {
			loadMemoryFromFile();
		} catch (...) {
			LogError("Invalid " << pwdFile << " file format");
			resetState();
			writeMemoryToFile();
		}
	}
}

void PasswordFile::prepareAttemptFile()
{
	std::string attemptFile = createDir(RW_DATA_DIR, m_user) + ATTEMPT_FILE;

	// check if attempt file exists
	// if not create it
	if (!fileExists(attemptFile)) {
		LogSecureDebug("PWD_DBG not found " << m_user << " attempt file. Creating.");
		writeAttemptToFile();
	} else {
		LogSecureDebug("PWD_DBG found " << m_user << " attempt file. Opening.");
		std::ifstream AttemptFile(attemptFile);

		if (!AttemptFile) {
			LogError("Failed to open " << m_user << " attempt file.");
			// ignore error
			return;
		}

		AttemptFile.read(reinterpret_cast<char *>(&m_attempt), sizeof(unsigned int));

		if (!AttemptFile) {
			LogError("Failed to read " << m_user << " attempt count.");
			// ignore error
			resetAttempt();
		}
	}
}

bool PasswordFile::fileExists(const std::string &filename) const
{
	struct stat buf;
	return ((stat(filename.c_str(), &buf) == 0));
}

bool PasswordFile::dirExists(const std::string &dirpath) const
{
	struct stat buf;
	return ((stat(dirpath.c_str(), &buf) == 0) && (((buf.st_mode) & S_IFMT) == S_IFDIR));
}

std::string PasswordFile::createDir(const std::string &dir, unsigned int user) const
{
	std::string User = std::to_string(user);
	return dir + "/" + User;
}

void PasswordFile::writeMemoryToFile() const
{
	PasswordFileBuffer pwdBuffer;
	LogSecureDebug("User: " << m_user << ", saving max_att: " << m_maxAttempt <<
				   ", history_size: " << m_maxHistorySize << ", m_expireTime: " <<
				   m_expireTime << ", m_expireTimeLeft: " << m_expireTimeLeft <<
				   ", isActive: " << m_passwordActive << ", isRcvActive: " <<
				   m_passwordRcvActive);
	//serialize password attributes
	Serialization::Serialize(pwdBuffer, CURRENT_FILE_VERSION);
	Serialization::Serialize(pwdBuffer, m_maxAttempt);
	Serialization::Serialize(pwdBuffer, m_maxHistorySize);
	Serialization::Serialize(pwdBuffer, m_expireTime);
	Serialization::Serialize(pwdBuffer, m_expireTimeLeft);
	Serialization::Serialize(pwdBuffer, m_passwordRcvActive);
	Serialization::Serialize(pwdBuffer, m_passwordActive);
	Serialization::Serialize(pwdBuffer, m_passwordCurrent);
	Serialization::Serialize(pwdBuffer, m_passwordHistory);
	std::string pwdFile = createDir(RW_DATA_DIR, m_user) + PASSWORD_FILE;
	pwdBuffer.Save(pwdFile);

	if (chmod(pwdFile.c_str(), FILE_MODE)) {
		LogError("Failed to chmod for " << pwdFile << " Error: " << errnoToString());
		Throw(PasswordException::ChmodError);
	}
}

void PasswordFile::loadMemoryFromFile()
{
	PasswordFileBuffer pwdBuffer;
	std::string pwdFile = createDir(RW_DATA_DIR, m_user) + PASSWORD_FILE;
	pwdBuffer.Load(pwdFile);
	unsigned int fileVersion = 0;
	Deserialization::Deserialize(pwdBuffer, fileVersion);

	if (fileVersion != CURRENT_FILE_VERSION)
		Throw(PasswordException::FStreamReadError);

	m_passwordHistory.clear();
	Deserialization::Deserialize(pwdBuffer, m_maxAttempt);
	Deserialization::Deserialize(pwdBuffer, m_maxHistorySize);
	Deserialization::Deserialize(pwdBuffer, m_expireTime);
	Deserialization::Deserialize(pwdBuffer, m_expireTimeLeft);
	Deserialization::Deserialize(pwdBuffer, m_passwordRcvActive);
	Deserialization::Deserialize(pwdBuffer, m_passwordActive);
	Deserialization::Deserialize(pwdBuffer, m_passwordCurrent);
	Deserialization::Deserialize(pwdBuffer, m_passwordHistory);
	LogSecureDebug("User: " << m_user << ", loaded max_att: " << m_maxAttempt <<
				   ", history_size: " << m_maxHistorySize << ", m_expireTime: " <<
				   m_expireTime << ", m_expireTimeLeft: " << m_expireTimeLeft <<
				   ", isActive: " << m_passwordActive << ", isRcvActive: " <<
				   m_passwordRcvActive);
}

bool PasswordFile::tryLoadMemoryFromOldFormatFile()
{
	struct stat oldFileStat;
	std::string oldVersionPwdFile = createDir(RW_DATA_DIR, m_user) + OLD_VERSION_PASSWORD_FILE;

	if (stat(oldVersionPwdFile.c_str(), &oldFileStat) != 0)
		return false;

	PasswordFileBuffer pwdBuffer;
	pwdBuffer.Load(oldVersionPwdFile);
	unsigned int fileVersion = 0;
	Deserialization::Deserialize(pwdBuffer, fileVersion);

	switch (fileVersion) {
	case 1:
	case 2:
	case 3:
		Deserialization::Deserialize(pwdBuffer, m_maxAttempt);
		Deserialization::Deserialize(pwdBuffer, m_maxHistorySize);
		Deserialization::Deserialize(pwdBuffer, m_expireTimeLeft);
		Deserialization::Deserialize(pwdBuffer, m_passwordActive);
		Deserialization::Deserialize(pwdBuffer, m_passwordCurrent);
		Deserialization::Deserialize(pwdBuffer, m_passwordHistory);

		m_expireTime = PASSWORD_INFINITE_EXPIRATION_DAYS;
		m_passwordRcvActive = false;
		break;
	default:
		LogError("Invaild password version: " << fileVersion);
		Throw(PasswordException::FStreamReadError);
	}
	return true;
}

void PasswordFile::writeAttemptToFile() const
{
	std::string attemptFile = createDir(RW_DATA_DIR, m_user) + ATTEMPT_FILE;
	std::ofstream AttemptFile(attemptFile, std::ofstream::trunc);

	if (!AttemptFile.good()) {
		LogError("Failed to open " << m_user << " attempt file.");
		Throw(PasswordException::FStreamOpenError);
	}

	AttemptFile.write(reinterpret_cast<const char *>(&m_attempt), sizeof(unsigned int));

	if (!AttemptFile) {
		LogError("Failed to write " << m_user << " attempt count.");
		Throw(PasswordException::FStreamWriteError);
	}

	AttemptFile.flush();
	if (::fsync(DPL::FstreamAccessors<std::ofstream>::GetFd(AttemptFile)) != 0)
		LogError("Failed to synchronize a file's state.");
	AttemptFile.close();
}

bool PasswordFile::isPasswordActive(unsigned int passwdType) const
{
	if (passwdType != AUTH_PWD_NORMAL)
		return false;

	return m_passwordActive;
}

void PasswordFile::setMaxHistorySize(unsigned int history)
{
	// put current password in history
	if (m_maxHistorySize == 0 && history > 0)
		m_passwordHistory.push_front(m_passwordCurrent);

	//setting history should be independent from password being set
	m_maxHistorySize = history;

	while (m_passwordHistory.size() > history)
		m_passwordHistory.pop_back();
}

unsigned int PasswordFile::getMaxHistorySize() const
{
	return m_maxHistorySize;
}

unsigned int PasswordFile::getAttempt() const
{
	return m_attempt;
}

void PasswordFile::resetAttempt()
{
	m_attempt = 0;
}

void PasswordFile::incrementAttempt()
{
	m_attempt++;
}

int PasswordFile::getMaxAttempt() const
{
	return m_maxAttempt;
}

void PasswordFile::setMaxAttempt(unsigned int maxAttempt)
{
	m_maxAttempt = maxAttempt;
}

bool PasswordFile::isPasswordReused(const std::string &password) const
{
	LogSecureDebug("Checking if " << m_user << " pwd is reused. HistorySize: " <<
				   m_passwordHistory.size() << ", MaxHistorySize: " << getMaxHistorySize());

	// go through history and check if password existed earlier
	if (std::any_of(
			m_passwordHistory.begin(),
			m_passwordHistory.end(),
			[&password](const IPasswordPtr & pwd) {
				return pwd->match(password);
			})) {
		LogSecureDebug(m_user << " passwords match!");
		return true;
	}

	LogSecureDebug("isPasswordReused: No passwords match, " << m_user <<
				   " password not reused.");
	return false;
}

void PasswordFile::setPassword(unsigned int passwdType, const std::string &password)
{
	if (passwdType != AUTH_PWD_NORMAL) {
		LogError("Password type is wrong.");
		return;
	}

	//replace current password with new one
	if (password.empty()) {
		m_passwordCurrent.reset(new NoPassword());
		m_passwordActive = false;
	} else {
		m_passwordCurrent.reset(new SHA256Password(password));
		//put current password to history
		m_passwordHistory.push_front(m_passwordCurrent);

		//erase last password if we exceed max history size
		if (m_passwordHistory.size() > getMaxHistorySize())
			m_passwordHistory.pop_back();

		m_passwordActive = true;
	}
}

bool PasswordFile::checkPassword(unsigned int passwdType, const std::string &password)
{
	if (passwdType != AUTH_PWD_NORMAL)
		return false;

	return m_passwordCurrent->match(password);
}

void PasswordFile::setExpireTime(unsigned int expireTime)
{
	m_expireTime = expireTime;
}

unsigned int PasswordFile::getExpireTime() const
{
	return m_expireTime;
}

void PasswordFile::setExpireTimeLeft(time_t expireTimeLeft)
{
	m_expireTimeLeft = expireTimeLeft;
}

unsigned int PasswordFile::getExpireTimeLeft() const
{
	if (m_expireTimeLeft != PASSWORD_INFINITE_EXPIRATION_TIME) {
		time_t timeLeft = m_expireTimeLeft - time(NULL);
		return (timeLeft < 0) ? 0 : static_cast<unsigned int>(timeLeft);
	} else {
		return PASSWORD_API_NO_EXPIRATION;
	}
}

bool PasswordFile::checkExpiration() const
{
	//return true if expired, else false
	return ((m_expireTimeLeft != PASSWORD_INFINITE_EXPIRATION_TIME) && (time(NULL) > m_expireTimeLeft));
}

bool PasswordFile::checkIfAttemptsExceeded() const
{
	return ((m_maxAttempt != PASSWORD_INFINITE_ATTEMPT_COUNT) && (m_attempt > m_maxAttempt));
}

bool PasswordFile::isIgnorePeriod() const
{
	TimePoint retryTimerStop = ClockType::now();
	TimeDiff diff = retryTimerStop - m_retryTimerStart;
	m_retryTimerStart = retryTimerStop;
	return (diff.count() < RETRY_TIMEOUT);
}

bool PasswordFile::isHistoryActive() const
{
	return (m_maxHistorySize != 0);
}

} //namespace SWBackend
} //namespace AuthPasswd
