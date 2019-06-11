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
#include <tz-backend/password-file.h>

#include <stdexcept>

#include <generic-backend/password-file-buffer.h>

extern "C" {

AuthPasswd::IPasswordFile* PasswordFileFactory(unsigned int user)
{
	return new AuthPasswd::TZBackend::PasswordFile(user);
}

} // extern "C"

namespace AuthPasswd {
namespace TZBackend {

PasswordFile::PasswordFile(unsigned int)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

void PasswordFile::writeMemoryToFile() const
{
	std::runtime_error("TZ-Backend is not implemented.");
}

void PasswordFile::writeAttemptToFile() const
{
	std::runtime_error("TZ-Backend is not implemented.");
}

void PasswordFile::setPassword(unsigned int, const std::string &)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

bool PasswordFile::checkPassword(unsigned int,
								 const std::string &)
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

void PasswordFile::setMaxHistorySize(unsigned int)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

unsigned int PasswordFile::getMaxHistorySize() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return -1;
}

unsigned int PasswordFile::getExpireTime() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return -1;
}

void PasswordFile::setExpireTime(unsigned int)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

void PasswordFile::setExpireTimeLeft(time_t)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

unsigned int PasswordFile::getExpireTimeLeft() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return -1;
}

unsigned int PasswordFile::getAttempt() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return -1;
}

void PasswordFile::resetAttempt()
{
	std::runtime_error("TZ-Backend is not implemented.");
}

void PasswordFile::incrementAttempt()
{
	std::runtime_error("TZ-Backend is not implemented.");
}

int PasswordFile::getMaxAttempt() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return -1;
}

void PasswordFile::setMaxAttempt(unsigned int)
{
	std::runtime_error("TZ-Backend is not implemented.");
}

bool PasswordFile::isPasswordActive(unsigned int) const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

bool PasswordFile::isPasswordReused(const std::string &) const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

bool PasswordFile::checkExpiration() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

bool PasswordFile::checkIfAttemptsExceeded() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

bool PasswordFile::isIgnorePeriod() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

bool PasswordFile::isHistoryActive() const
{
	std::runtime_error("TZ-Backend is not implemented.");
	return false;
}

} //namespace TZBackend
} //namespace AuthPasswd
