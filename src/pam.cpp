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

#include <string>
#include <vector>

#include <security/pam_ext.h>
#include <security/pam_modules.h>

#include <klay/pam.h>
#include <klay/exception.h>

namespace klay {

PAM::PAM(const std::string& service, const std::string& user)
{
	struct pam_conv pamc;

	int error = ::pam_start(service.c_str(), user.c_str(), &pamc, &pamh);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

PAM::~PAM()
{
	int error = ::pam_end(pamh, PAM_SUCCESS);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

void PAM::setData(const std::string &name, void* data, void (*cleanup)(pam_handle_t* pamh, void* data, int error))
{
	int error = ::pam_set_data(pamh, name.c_str(), data, cleanup);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

const void* PAM::getData(const std::string &name) const
{
	const void* ret;
	int error = ::pam_get_data(pamh, name.c_str(), &ret);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
	return ret;
}

void PAM::setItem(int item, void* data)
{
	int error = ::pam_set_item(pamh, item, data);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

const void* PAM::getItem(int item) const
{
	const void* ret;
	int error = ::pam_get_item(pamh, item, &ret);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
	return ret;
}

const std::string PAM::getUser(const std::string &prompt) const
{
	const char* user;
	int error = ::pam_get_user(pamh, &user, prompt.c_str());
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
	return std::string(user);
}

void PAM::putEnv(const std::string &name_value)
{
	int error = ::pam_putenv(pamh, name_value.c_str());
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

const std::string PAM::getEnv(const std::string &name) const
{
	const char* value = ::pam_getenv(pamh, name.c_str());
	if (value == NULL) {
		throw klay::Exception("PAM Error");
	}
	return value;
}

const std::vector<std::string> PAM::getEnvList() const
{
	std::vector<std::string> ret;
	char** array = ::pam_getenvlist(pamh);
	if (array == NULL) {
		throw klay::Exception("PAM Error");
	}
	for (int i = 0; array[i] != NULL; i++) {
		ret.push_back(array[i]);
	}
	return ret;
}

void PAM::syslog(const std::string &log, int priority)
{
	::pam_syslog(pamh, priority, "%s", log.c_str());
}


int PAM::authenticate(int flags)
{
	return ::pam_authenticate(pamh, flags);
}

int PAM::setCredential(int flags)
{
	return ::pam_setcred(pamh, flags);
}

int PAM::accountManagement(int flags)
{
	return ::pam_acct_mgmt(pamh, flags);
}

int PAM::changeAuthenticationToken(int flags)
{
	return ::pam_chauthtok(pamh, flags);
}

void PAM::openSession(int flags)
{
	int error = ::pam_open_session(pamh, flags);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

void PAM::closeSession(int flags)
{
	int error = ::pam_close_session(pamh, flags);
	if (error != PAM_SUCCESS) {
		throw klay::Exception("PAM Error");
	}
}

} // namespace klay
