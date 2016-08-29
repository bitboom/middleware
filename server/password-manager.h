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

#ifndef __DPM_PASSWORD_MANAGER_H__
#define __DPM_PASSWORD_MANAGER_H__
#include <sys/types.h>

#include <vector>
#include <string>

#include <auth-passwd-admin.h>

class PasswordManager {
public:
	typedef password_quality_type QualityType;

	PasswordManager(uid_t uid);
	~PasswordManager();

	void setQuality(QualityType quality);
	void setMinimumLength(int value);
	void setMinimumComplexCharacters(int value);
	void setMaximumFailedForWipe(int value);
	void setExpires(int value);
	void setHistory(int value);
	void setPattern(const std::string &pattern);
	void deletePatern();
	void setMaximumCharacterOccurrences(int value);
	void setMaximumNumericSequenceLength(int value);
	void setForbiddenStrings(const std::vector<std::string> &forbiddenStrings);
	void resetPassword(const std::string& password);
	void enforce();

private:
	uid_t user;
	policy_h *p_policy;
};
#endif //!__DPM_PASSWORD_MANAGER_H__
