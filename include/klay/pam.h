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

#ifndef __RUNTIME_PAM_H__
#define __RUNTIME_PAM_H__

#include <string>
#include <vector>
#include <memory>

#include <syslog.h>
#include <security/pam_appl.h>

namespace runtime {

class PAM final {
public:
	PAM(PAM&&) = delete;
	PAM(const PAM&) = delete;
	PAM(const std::string& service, const std::string& user);
	~PAM();

	PAM& operator=(const PAM&) = delete;
	PAM& operator=(PAM &&) = delete;

	void setData(const std::string &name, void* data, void (*cleanup)(pam_handle_t* pamh, void* data, int error));
	const void* getData(const std::string &name) const;

	void setItem(int item, void* data);
	const void* getItem(int item) const;

	const std::string getUser(const std::string &prompt = "") const;

	void putEnv(const std::string &name_value);
	const std::string getEnv(const std::string &name) const;
	const std::vector<std::string> getEnvList() const;

	void syslog(const std::string &log, int priority = LOG_ERR);

	int authenticate(int flags);
	int setCredential(int flags);
	int accountManagement(int flags);
	int changeAuthenticationToken(int flags);
	void openSession(int flags);
	void closeSession(int flags);

private:
	pam_handle_t* pamh;
};

} // namespace runtime
#endif // __RUNTIME_PAM_H__
