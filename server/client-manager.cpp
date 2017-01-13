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

#include <klay/exception.h>

#include "client-manager.h"
#include "policy-storage.h"

PolicyClient::PolicyClient(PolicyControlContext& context) :
	PolicyClient(context.getPeerPid(), context.getPeerUid())
{
}

PolicyClient::PolicyClient(pid_t pid, uid_t user)
{
	char pkgid[PATH_MAX];

	if (aul_app_get_pkgid_bypid_for_uid(pid, pkgid, PATH_MAX, user) != 0) {
		int fd = ::open(std::string("/proc/" + std::to_string(pid) + "/cmdline").c_str(), O_RDONLY);
		if (fd == -1) {
			throw runtime::Exception("Unknown PID");
		}

		ssize_t ret, bytes = 0;
		do {
			ret = ::read(fd, &pkgid[bytes], PATH_MAX);
			if (ret != -1) {
				bytes += ret;
			}
		} while ((ret == -1) && (errno == EINTR));

		while (::close(fd) == -1 && errno == EINTR);

		if (ret == -1) {
			throw runtime::Exception("Failed to get admin info");
		}

		pkgid[bytes] = '\0';
	}

	name = pkgid;
	uid = user;
}

PolicyClient::~PolicyClient()
{
}

Observerable ClientManager::observers;

void ClientManager::loadAdministrators()
{
	std::vector<uid_t> domains = getManagedDomainList();
	observers.notify(domains);
}

void ClientManager::registerAdministrator(const std::string& name, uid_t domain)
{
	std::string selectQuery = "SELECT * FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt0 = PolicyStorage::prepare(selectQuery);
	stmt0.bind(1, name);
	stmt0.bind(2, static_cast<int>(domain));
	if (stmt0.step()) {
		throw runtime::Exception("Admin client already registered");
	}

	std::string key = "Not supported";

	std::string insertQuery = "INSERT INTO admin (pkg, uid, key, removable) VALUES (?, ?, ?, ?)";
	DataSet stmt = PolicyStorage::prepare(insertQuery);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(domain));
	stmt.bind(3, key);
	stmt.bind(4, true);
	if (!stmt.exec()) {
		throw runtime::Exception("I/O failure");
	}
}

void ClientManager::deregisterAdministrator(const std::string& name, uid_t domain)
{
	std::string query = "DELETE FROM admin WHERE pkg = ? AND uid = ?";
	DataSet stmt = PolicyStorage::prepare(query);
	stmt.bind(1, name);
	stmt.bind(2, static_cast<int>(domain));
	if (!stmt.exec()) {
		throw runtime::Exception("Unknown administrator");
	}

	observers.notify({domain});
}

void ClientManager::addEventListener(Observer* listener)
{
	observers.attach(listener);
}

void ClientManager::removeEventListener(Observer* listener)
{
}

std::vector<uid_t> ClientManager::getManagedDomainList()
{
	std::vector<uid_t> managedDomains;
	std::string query = "SELECT DISTINCT uid FROM admin;";
	DataSet stmt = PolicyStorage::prepare(query);
	while (stmt.step()) {
		managedDomains.push_back(stmt.getColumn(0).getInt());
	}

	return managedDomains;
}
