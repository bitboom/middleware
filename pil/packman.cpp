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

#include "packman.h"

namespace {

int PackageEventCallback(uid_t uid, int id, const char* type, const char* name,
						 const char* key, const char* val, const void* msg, void* data)
{
	return 0;
}

int PackageListCallback(pkgmgrinfo_pkginfo_h handle, void *data)
{
	char* pkgid = nullptr;
	::pkgmgrinfo_pkginfo_get_pkgid(handle, &pkgid);

	std::vector<std::string>* packageList = static_cast<std::vector<std::string>*>(data);
	packageList->push_back(pkgid);

	return 0;
}

} // namespace

PackageInfo::PackageInfo(const std::string& pkgid, uid_t uid) :
	user(uid), handle(nullptr)
{
	if (uid == 0) {
		uid = getuid();
	}

	if (::pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid.c_str(), user, &handle) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid Package Id");
	}
}

PackageInfo::~PackageInfo()
{
	::pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
}

std::string PackageInfo::getType() const
{
	char *pkgtype;
	if (::pkgmgrinfo_pkginfo_get_type(handle, &pkgtype) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return pkgtype;
}

std::string PackageInfo::getLabel() const
{
	char *label;
	if (::pkgmgrinfo_pkginfo_get_label(handle, &label) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return label;
}

std::string PackageInfo::getIcon() const
{
	char *icon;
	if (::pkgmgrinfo_pkginfo_get_icon(handle, &icon) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return icon;
}

std::string PackageInfo::getDescription() const
{
	char *desc;
	if (::pkgmgrinfo_pkginfo_get_description(handle, &desc) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return desc;
}

std::string PackageInfo::getAuthorName() const
{
	char *name;
	if (::pkgmgrinfo_pkginfo_get_author_name(handle, &name) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return name;
}

std::string PackageInfo::getAuthorEmail() const
{
	char *email;
	if (::pkgmgrinfo_pkginfo_get_author_email(handle, &email) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return email;
}

std::string PackageInfo::getAuthorHref() const
{
	char *href;
	if (::pkgmgrinfo_pkginfo_get_author_name(handle, &href) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return href;
}

std::string PackageInfo::getVersion() const
{
	char *version;
	if (::pkgmgrinfo_pkginfo_get_version(handle, &version) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return version;
}

std::string PackageInfo::getApiVersion() const
{
	char *api;
	if (::pkgmgrinfo_pkginfo_get_api_version(handle, &api) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return api;
}

std::string PackageInfo::getMainAppId() const
{
	char *mainappid;
	if (::pkgmgrinfo_pkginfo_get_mainappid(handle, &mainappid) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return mainappid;
}

bool PackageInfo::isSystem() const
{
	bool ret;
	if (::pkgmgrinfo_pkginfo_is_system(handle, &ret) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return ret;
}

bool PackageInfo::isRemovable() const
{
	bool ret;
	if (::pkgmgrinfo_pkginfo_is_removable(handle, &ret) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return ret;
}

bool PackageInfo::isPreload() const
{
	bool ret;
	if (::pkgmgrinfo_pkginfo_is_preload(handle, &ret) != PMINFO_R_OK) {
		throw runtime::Exception("Invalid operation");
	}

	return ret;
}

PackageManager::PackageManager() :
	nativeRequestHandle(nullptr), nativeListenHandle(nullptr)
{
	nativeRequestHandle = ::pkgmgr_client_new(PC_REQUEST);
	if (nativeRequestHandle == nullptr) {
		throw runtime::Exception("No package manager request instance");
	}

	nativeListenHandle = ::pkgmgr_client_new(PC_LISTENING);
	if (nativeListenHandle == nullptr) {
		::pkgmgr_client_free(nativeRequestHandle);
		throw runtime::Exception("No package manager listening instance");
	}
}

PackageManager::~PackageManager()
{
	::pkgmgr_client_free(nativeRequestHandle);
	::pkgmgr_client_free(nativeListenHandle);
}

PackageManager& PackageManager::instance()
{
	static PackageManager __instance__;
	return __instance__;
}

void PackageManager::setEventCallback(pkgmgr_handler callback, void* user_data)
{
	int ret;
	ret = ::pkgmgr_client_listen_status(nativeListenHandle, callback, user_data);
	if (ret < 0) {
		throw runtime::Exception("Failed to set package event callback");
	}
}

void PackageManager::unsetEventCallback()
{
	int ret;
	ret = ::pkgmgr_client_remove_listen_status(nativeListenHandle);
	if (ret < 0) {
		throw runtime::Exception("Failed to unset package event callback");
	}
}

void PackageManager::installPackage(const std::string& pkgpath, const uid_t user)
{
	int ret = ::pkgmgr_client_usr_install(nativeRequestHandle, NULL, NULL, pkgpath.c_str(), NULL, PM_QUIET, PackageEventCallback, nullptr, user);
	if (ret < PKGMGR_R_OK) {
		throw runtime::Exception("Package installation failed");
	}
}

void PackageManager::uninstallPackage(const std::string& pkgid, const uid_t user)
{
	std::string pkgtype;

	PackageInfo pkgInfo(pkgid, user);
	pkgtype = pkgInfo.getType();

	int ret = ::pkgmgr_client_usr_uninstall(nativeRequestHandle, pkgtype.c_str(), pkgid.c_str(), PM_QUIET, PackageEventCallback, nullptr, user);
	if (ret < PKGMGR_R_OK) {
		throw runtime::Exception("Package manager exception");
	}
}

std::vector<std::string> PackageManager::getPackageList(const uid_t user)
{
	std::vector<std::string> packageList;

	if (::pkgmgrinfo_pkginfo_get_usr_list(PackageListCallback, &packageList, user) != PMINFO_R_OK) {
		throw runtime::Exception("Error in pkgmgrinfo_pkginfo_get_list");
	}

	return packageList;
}

void PackageManager::setModeRestriction(int mode, uid_t user)
{
	if (::pkgmgr_client_usr_set_restriction_mode(nativeRequestHandle,
												 mode,
												 user) != PKGMGR_R_OK) {
		throw runtime::Exception("Failed to set package restriction mode");
	}
}

void PackageManager::unsetModeRestriction(int mode, uid_t user)
{
	if (::pkgmgr_client_usr_unset_restriction_mode(nativeRequestHandle,
												   mode,
												   user) != PKGMGR_R_OK) {
		throw runtime::Exception("Failed to unset package mode restriction");
	}
}

int PackageManager::getModeRestriction(uid_t user)
{
	int mode;
	if (::pkgmgr_client_usr_get_restriction_mode(nativeRequestHandle,
												 &mode,
												 user) != PKGMGR_R_OK) {
		throw runtime::Exception("Failed to get package restriction mode");
	}

	return mode;
}
