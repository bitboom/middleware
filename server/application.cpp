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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include <privilege_manager.h>
#include <privilege_info.h>
#include <klay/exception.h>
#include <klay/audit/logger.h>

#include "packman.h"
#include "launchpad.h"
#include "policy-builder.h"

#include "application.hxx"

namespace DevicePolicyManager {

ApplicationPolicy::ApplicationPolicy(PolicyControlContext& ctxt) :
	context(ctxt)
{
	context.expose(this, "", (int)(ApplicationPolicy::getModeRestriction)());
	context.expose(this, "", (int)(ApplicationPolicy::setModeRestriction)(int));
	context.expose(this, "", (int)(ApplicationPolicy::unsetModeRestriction)(int));

	context.expose(this, "", (int)(ApplicationPolicy::addPrivilegeToBlacklist)(int, std::string));
	context.expose(this, "", (int)(ApplicationPolicy::removePrivilegeFromBlacklist)(int, std::string));
	context.expose(this, "", (int)(ApplicationPolicy::checkPrivilegeIsBlacklisted)(int, std::string));

	context.expose(this, "", (int)(ApplicationPolicy::installPackage)(std::string));
	context.expose(this, "", (int)(ApplicationPolicy::uninstallPackage)(std::string));
}

ApplicationPolicy::~ApplicationPolicy()
{
}

int ApplicationPolicy::installPackage(const std::string& pkgpath)
{
	try {
		PackageManager& packman = PackageManager::instance();
		packman.installPackage(pkgpath, context.getPeerUid());
	} catch (runtime::Exception& e) {
		ERROR("Exception on package installation: " + pkgpath);
		return -1;
	}

	return 0;
}

int ApplicationPolicy::uninstallPackage(const std::string& pkgid)
{
	try {
		PackageManager& packman = PackageManager::instance();
		packman.uninstallPackage(pkgid, context.getPeerUid());
	} catch (runtime::Exception& e) {
		ERROR("Exception on package uninstallation: " + pkgid);
		return -1;
	}

	return 0;
}

int ApplicationPolicy::setModeRestriction(int mode)
{
	try {
		PackageManager& packman = PackageManager::instance();
		packman.setModeRestriction(mode, context.getPeerUid());
	} catch (runtime::Exception& e) {
		ERROR("Failed to set mode restriction");
		return -1;
	}

	return 0;
}

int ApplicationPolicy::unsetModeRestriction(int mode)
{
	try {
		PackageManager& packman = PackageManager::instance();
		packman.unsetModeRestriction(mode, context.getPeerUid());
	} catch (runtime::Exception& e) {
		ERROR("Failed to unset mode restriction");
		return -1;
	}

	return 0;
}

int ApplicationPolicy::getModeRestriction()
{
	try {
		PackageManager& packman = PackageManager::instance();
		return packman.getModeRestriction(context.getPeerUid());
	} catch (runtime::Exception& e) {
		ERROR("Failed to get mode restriction");
		return -1;
	}
}

int ApplicationPolicy::addPrivilegeToBlacklist(int type, const std::string& privilege)
{
	GList* privilegeList = NULL;
	privilegeList = g_list_append(privilegeList, const_cast<char*>(privilege.c_str()));
	privilege_manager_package_type_e pkgType = type ? PRVMGR_PACKAGE_TYPE_CORE : PRVMGR_PACKAGE_TYPE_WRT;
	int ret = privilege_manager_set_black_list(context.getPeerUid(), pkgType, privilegeList);
	g_list_free(privilegeList);
	if (ret !=PRVMGR_ERR_NONE) {
		return -1;
	}

	return 0;
}

int ApplicationPolicy::removePrivilegeFromBlacklist(int type, const std::string& privilege)
{
	GList* privilegeList = NULL;
	privilegeList = g_list_append(privilegeList, const_cast<char*>(privilege.c_str()));

	privilege_manager_package_type_e pkgType = type ? PRVMGR_PACKAGE_TYPE_CORE : PRVMGR_PACKAGE_TYPE_WRT;
	int ret = privilege_manager_unset_black_list(context.getPeerUid(), pkgType, privilegeList);
	g_list_free(privilegeList);
	if (ret != PRVMGR_ERR_NONE) {
		return -1;
	}

	return 0;
}

int ApplicationPolicy::checkPrivilegeIsBlacklisted(int type, const std::string& privilege)
{
	GList* blacklist = NULL;

	privilege_manager_package_type_e pkgType = type ? PRVMGR_PACKAGE_TYPE_CORE : PRVMGR_PACKAGE_TYPE_WRT;
	int ret = privilege_info_get_black_list(context.getPeerUid(), pkgType, &blacklist);
	if (ret != PRVMGR_ERR_NONE) {
		return -1;
	}

	for (GList* l = blacklist; l != NULL; l = l->next) {
		char *name = (char *)l->data;
		if (privilege == name) {
			g_list_free(blacklist);
			return true;
		}
	}
	g_list_free(blacklist);
	return false;
}

DEFINE_POLICY(ApplicationPolicy);

} // namespace DevicePolicyManager
