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

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <functional>

#include <aul.h>
#include <cynara-client.h>
#include <cynara-session.h>

#include "server.h"
#include "policy-builder.h"
#include "client-manager.h"

#include "exception.h"
#include "filesystem.h"
#include "audit/logger.h"

using namespace std::placeholders;

namespace {

const std::string POLICY_MANAGER_ADDRESS = "/tmp/.device-policy-manager.sock";
const std::string POLICY_ACCESS_POINT_PATH = "/var/run/dpm";
const std::string DEVICE_ADMIN_REPOSITORY = DB_PATH;

std::string GetPackageId(uid_t uid, pid_t pid)
{
	char pkgid[PATH_MAX];

	if (aul_app_get_pkgid_bypid_for_uid(pid, pkgid, PATH_MAX, uid) != 0) {
		throw runtime::Exception("Unknown PID");
	}

	return pkgid;
}

} // namespace

Server::Server()
{
	policyManager.reset(new PolicyManager(POLICY_ACCESS_POINT_PATH));
	adminManager.reset(new DeviceAdministratorManager(DEVICE_ADMIN_REPOSITORY));
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
	policyManager->prepareGlobalPolicy();

	int index = 0;
	uid_t uids[32];
	DeviceAdministratorManager::DeviceAdministratorList::iterator iter = adminManager->begin();
	while (iter != adminManager->end()) {
		int i = 0;
		const DeviceAdministrator& admin = *iter;
		uid_t uid = admin.getUid();
		while ((i < index) && (uids[i] != uid)) i++;

		if (i == index) {
			uids[index++] = uid;
			policyManager->prepareUserPolicy(uid);
		}

		policyManager->populateStorage(admin.getName(), admin.getUid(), true);
		++iter;
	}

	PolicyBuild(*this);

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

int Server::setPolicy(const std::string& name, int value, const std::string& event, const std::string& info)
{
	uid_t uid = getPeerUid();
	try {
		std::string pkgid = GetPackageId(uid, getPeerPid());
		if (policyManager->setPolicy(pkgid, uid, name, value)) {
			if (event.empty() == false) {
				service->notify(event, info);
			}
		}
	} catch (runtime::Exception& e) {
		ERROR(e.what());
		return -1;
	}

	return 0;
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

int Server::enroll(const std::string& name, uid_t uid)
{
	adminManager->enroll(name, uid);
	policyManager->prepareUserPolicy(uid);
	policyManager->populateStorage(name, uid, true);

	return 0;
}

int Server::disenroll(const std::string& name, uid_t uid)
{
	adminManager->disenroll(name, uid);
	policyManager->removeStorage(name, uid);

	return 0;
}
