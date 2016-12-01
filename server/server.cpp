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

#include <functional>

#include <cynara-client.h>
#include <cynara-session.h>

#include "server.h"
#include "policy-builder.h"

#include "exception.h"
#include "filesystem.h"
#include "audit/logger.h"

using namespace std::placeholders;

namespace {

const std::string POLICY_MANAGER_ADDRESS = "/tmp/.device-policy-manager.sock";
const std::string POLICY_STORAGE_PATH = "/opt/dbspace/.dpm.db";

} // namespace

Server::Server()
{
	policyManager.reset(new PolicyManager(POLICY_STORAGE_PATH));
	service.reset(new rmi::Service(POLICY_MANAGER_ADDRESS));

	service->setPrivilegeChecker(std::bind(&Server::checkPeerPrivilege, this, _1, _2));

	service->registerParametricMethod(this, "", (runtime::FileDescriptor)(Server::registerNotificationSubscriber)(std::string));
	service->registerParametricMethod(this, "", (int)(Server::unregisterNotificationSubscriber)(std::string, int));
}

Server::~Server()
{
}

void Server::run()
{
	PolicyBuild(*this);

	policyManager->apply();

	::umask(0);
	service->start(true);
}

void Server::terminate()
{
	service->stop();
}

runtime::FileDescriptor Server::registerNotificationSubscriber(const std::string& name)
{
	return runtime::FileDescriptor(service->subscribeNotification(name), true);
}

int Server::unregisterNotificationSubscriber(const std::string& name, int id)
{
	return service->unsubscribeNotification(name, id);
}

bool Server::checkPeerPrivilege(const rmi::Credentials& cred, const std::string& privilege)
{
	cynara *p_cynara;

	if (privilege.empty()) {
		return true;
	}

	if (::cynara_initialize(&p_cynara, NULL) != CYNARA_API_SUCCESS) {
		ERROR("Failure in cynara API");
		return false;
	}

	if (::cynara_check(p_cynara, cred.security.c_str(), "",
					   std::to_string(cred.uid).c_str(),
					   privilege.c_str()) != CYNARA_API_ACCESS_ALLOWED) {
		::cynara_finish(p_cynara);
		ERROR("Access denied: " + cred.security + " : " + privilege);
		return false;
	}

	::cynara_finish(p_cynara);

	return true;
}

