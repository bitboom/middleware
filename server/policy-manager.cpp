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

#include <climits>
#include <string>
#include <unordered_map>
#include <iostream>

#include <klay/exception.h>
#include <klay/filesystem.h>
#include <klay/audit/logger.h>

#include "client-manager.h"
#include "policy-manager.h"

namespace {

const std::string schema = \
	"CREATE TABLE IF NOT EXISTS admin ("                                  \
	"    id        INTEGER PRIMARY KEY AUTOINCREMENT, "                   \
	"    pkg       TEXT NOT NULL, "                                       \
	"    uid       INTEGER, "                                             \
	"    key       TEXT, "                                                \
	"    removable INTEGER"                                               \
	");"                                                                  \
	"CREATE TABLE IF NOT EXISTS managed_policy ("                         \
	"    id        INTEGER PRIMARY KEY AUTOINCREMENT, "                   \
	"    aid       INTEGER,"                                              \
	"    pid       INTEGER,"                                              \
	"    value     INTEGER"                                               \
	");"                                                                  \
	"CREATE TABLE IF NOT EXISTS policy_definition ("                      \
    "    id        INTEGER PRIMARY KEY, "                                 \
	"    scope     INTEGER, "                                             \
	"    name      TEXT NOT NULL, "                                       \
	"    ivalue    INTEGER"                                               \
	");"                                                                  \
	"CREATE TRIGGER IF NOT EXISTS prepare_storage AFTER INSERT ON admin " \
	"FOR EACH ROW "                                                       \
	"BEGIN "                                                              \
	"INSERT INTO managed_policy(aid, pid, value) "                        \
	"SELECT NEW.ID, policy_definition.id, policy_definition.ivalue "      \
	"FROM policy_definition; "                                            \
	"END;"                                                                \
	"CREATE TRIGGER IF NOT EXISTS remove_storage AFTER DELETE ON admin "  \
	"FOR EACH ROW "                                                       \
	"BEGIN "                                                              \
	"DELETE FROM managed_policy WHERE managed_policy.aid = OLD.ID; "      \
	"END;";

bool StateComparator(int v1, int v2)
{
	return ((v1 != v2) && (v2 == 0)) ? true : false;
}

bool RestrictionComparator(int v1, int v2)
{
	return ((v1 != v2) && (v2 == 1)) ? true : false;
}

bool MinimizeIntegerComparator(int v1, int v2)
{
	return v2 < v1 ? true : false;
}

bool MaximizeIntegerComparator(int v1, int v2)
{
	return v1 < v2 ? true : false;
}

bool PatternComparator(const std::string& v1, const std::string& v2)
{
	return false;
}

} // namespace

#define USER_POLICY(_n_, _v_, _c_)	\
{ _n_, ManagedPolicy(_n_, 1, _v_, _c_) }

#define GLOBAL_POLICY(_n_, _v_, _c_) \
{ _n_, ManagedPolicy(_n_, 0, _v_, _c_) }

std::unordered_map<std::string, ManagedPolicy> PolicyManager::managedPolicyMap = {
USER_POLICY("password-history",                       int(0),          MaximizeIntegerComparator),
USER_POLICY("password-minimum-length",                int(0),          MaximizeIntegerComparator),
USER_POLICY("password-minimum-complexity",            int(0),          MaximizeIntegerComparator),
USER_POLICY("password-inactivity-timeout",            int(INT_MAX),    MinimizeIntegerComparator),
USER_POLICY("password-expired",                       int(INT_MAX),    MinimizeIntegerComparator),
USER_POLICY("password-maximum-failure-count",         int(INT_MAX),    MinimizeIntegerComparator),
USER_POLICY("password-numeric-sequences-length",      int(INT_MAX),    MinimizeIntegerComparator),
USER_POLICY("password-maximum-character-occurrences", int(INT_MAX),    MinimizeIntegerComparator),
USER_POLICY("password-quality",                       int(0),          MaximizeIntegerComparator),
USER_POLICY("password-recovery",                      int(0),          RestrictionComparator),
USER_POLICY("password-pattern",                       std::string(""), PatternComparator),
USER_POLICY("browser",                                int(1),          StateComparator),
USER_POLICY("clipboard",                              int(1),          StateComparator),
GLOBAL_POLICY("bluetooth",                              int(1),          StateComparator),
GLOBAL_POLICY("bluetooth-tethering",                    int(1),          StateComparator),
GLOBAL_POLICY("bluetooth-desktop-connectivity",         int(1),          StateComparator),
GLOBAL_POLICY("bluetooth-pairing",                      int(1),          StateComparator),
GLOBAL_POLICY("bluetooth-uuid-restriction",             int(0),          RestrictionComparator),
GLOBAL_POLICY("bluetooth-device-restriction",           int(0),          RestrictionComparator),
GLOBAL_POLICY("camera",                                 int(1),          StateComparator),
GLOBAL_POLICY("external-storage",                       int(1),          StateComparator),
GLOBAL_POLICY("location",                               int(1),          StateComparator),
GLOBAL_POLICY("messaging",                              int(1),          StateComparator),
GLOBAL_POLICY("microphone",                             int(1),          StateComparator),
GLOBAL_POLICY("popimap-email",                          int(1),          StateComparator),
GLOBAL_POLICY("usb-debugging",                          int(1),          StateComparator),
GLOBAL_POLICY("usb-tethering",                          int(1),          StateComparator),
GLOBAL_POLICY("wifi",                                   int(1),          StateComparator),
GLOBAL_POLICY("wifi-hotspot",                           int(1),          StateComparator),
GLOBAL_POLICY("wifi-profile-change",                    int(1),          StateComparator),
GLOBAL_POLICY("wifi-ssid-restriction",                  int(0),          RestrictionComparator)
};

std::atomic<unsigned int> ManagedPolicy::IVariantStorage::sequencer(1);

PolicyManager::PolicyManager(const std::string& path) :
	connection(path, database::Connection::ReadWrite | database::Connection::Create)
{
	initializeStorage();
}

PolicyManager::~PolicyManager()
{
}

std::vector<uid_t> PolicyManager::getManagedDomainList()
{
	std::vector<uid_t> managedDomains;
	std::string query0 = "SELECT DISTINCT uid FROM admin;";
	database::Statement stmt0(connection, query0);
	while (stmt0.step()) {
		managedDomains.push_back(stmt0.getColumn(0).getInt());
	}

	return managedDomains;
}

void PolicyManager::initializeStorage()
{
	connection.exec(schema);
	std::string query = "SELECT id FROM policy_definition;";
	database::Statement stmt(connection, query);
	if (stmt.step() == false) {
		for (auto &it : managedPolicyMap) {
			ManagedPolicy &policy = it.second;
			policy.declare(connection);
		}
	}
}

void PolicyManager::apply()
{
	std::vector<uid_t> managedDomains = getManagedDomainList();

	for (auto &it : managedPolicyMap) {
		ManagedPolicy &policy = it.second;
		policy.apply(connection, managedDomains);
	}
}

int PolicyManager::prepareStorage(const std::string& admin, uid_t uid)
{
	std::string selectQuery = "SELECT * FROM admin WHERE pkg = ? AND uid = ?";
	database::Statement stmt0(connection, selectQuery);
	stmt0.bind(1, admin);
	stmt0.bind(2, static_cast<int>(uid));
	if (stmt0.step()) {
		ERROR("Admin client already registered");
		return -1;
	}

	std::string key = "Not supported";

	std::string insertQuery = "INSERT INTO admin (pkg, uid, key, removable) VALUES (?, ?, ?, ?)";
	database::Statement stmt(connection, insertQuery);
	stmt.bind(1, admin);
	stmt.bind(2, static_cast<int>(uid));
	stmt.bind(3, key);
	stmt.bind(4, true);
	if (!stmt.exec()) {
		ERROR("I/O failure");
		return -1;
	}

	return 0;
}

int PolicyManager::removeStorage(const std::string& admin, uid_t domain)
{
	std::string query = "DELETE FROM admin WHERE pkg = ? AND uid = ?";
	database::Statement stmt(connection, query);
	stmt.bind(1, admin);
	stmt.bind(2, static_cast<int>(domain));
	if (!stmt.exec()) {
		ERROR("Unknown device admin client: " + admin);
		return -1;
	}

	for (auto &it : managedPolicyMap) {
		ManagedPolicy &policy = it.second;
		policy.apply(connection, std::vector<uid_t>({domain}));
	}

	return 0;
}
