/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        TrustAnchor.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Implementation of trust anchor
 */
#include "TrustAnchor.h"

#include <climits>
#include <cerrno>
#include <ctime>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include <set>
#include <vector>

#include <klay/filesystem.h>
#include <klay/audit/logger.h>

#include "Certificate.h"
#include "Exception.h"

namespace transec {

namespace {

const std::string BASE_USR_PATH(CERTSVC_TRANSEC_USR_DIR);
const std::string BASE_GLOBAL_PATH(CERTSVC_TRANSEC_GLOBAL_DIR);
const std::string TRANSEC_BUNDLE_PATH(CERTSVC_TRANSEC_BUNDLE);
const std::string SYS_CERTS_PATH(TZ_SYS_CA_CERTS);
const std::string SYS_BUNDLE_PATH(TZ_SYS_CA_BUNDLE);
const std::string MOUNT_POINT_CERTS(TZ_SYS_CA_CERTS);
const std::string MOUNT_POINT_BUNDLE(TZ_SYS_CA_BUNDLE);
const std::string BUNDLE_NAME("ca-bundle.pem");
const std::string NEW_LINE("\n");

} // namespace anonymous

class TrustAnchor::Impl {
public:
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir,
				  uid_t uid) noexcept;
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir) noexcept;
	virtual ~Impl(void) = default;

	int install(bool withSystemCerts) noexcept;
	int uninstall(bool isRollback = false) noexcept;
	int launch(bool withSystemCerts);

private:
	void preInstall(void) const;
	void linkTo(const std::string &src, const std::string &dst) const;
	void makeCustomBundle(bool withSystemCerts);
	std::string readLink(const std::string &path) const;
	std::string getUniqueHashName(const std::string &hashName) const;
	bool isSystemCertsModified(void) const;

	std::string m_packageId;
	std::string m_appCertsPath;
	uid_t m_uid;

	std::string m_customBasePath;
	std::string m_customCertsPath;
	std::string m_customBundlePath;

	std::set<std::string> m_customCertNameSet;
	std::vector<std::string> m_customCertsData;
};

TrustAnchor::Impl::Impl(const std::string &packageId,
						const std::string &certsDir,
						uid_t uid) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_uid(uid),
	m_customBasePath(BASE_USR_PATH + "/" +
					 std::to_string(static_cast<int>(uid)) + "/" +
					 packageId),
	m_customCertsPath(m_customBasePath + "/certs"),
	m_customBundlePath(m_customBasePath + "/bundle"),
	m_customCertNameSet(),
	m_customCertsData() {}

TrustAnchor::Impl::Impl(const std::string &packageId,
						const std::string &certsDir) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_uid(-1),
	m_customBasePath(BASE_GLOBAL_PATH + "/" + packageId),
	m_customCertsPath(m_customBasePath + "/certs"),
	m_customBundlePath(m_customBasePath + "/bundle"),
	m_customCertNameSet(),
	m_customCertsData() {}

std::string TrustAnchor::Impl::readLink(const std::string &path) const
{
	std::vector<char> buf(PATH_MAX);
	ssize_t count = readlink(path.c_str(), buf.data(), buf.size());
	return std::string(buf.data(), (count > 0) ? count : 0);
}

void TrustAnchor::Impl::linkTo(const std::string &src,
							   const std::string &dst) const
{
	errno = 0;
	int ret = ::symlink(src.c_str(), dst.c_str());
	if (ret != 0)
		throw std::logic_error("Fail to link " + src + " -> " + dst +
							   "[" + std::to_string(errno) + "]");
}

void TrustAnchor::Impl::preInstall(void) const
{
	runtime::File customBaseDir(this->m_customBasePath);
	if (customBaseDir.exists()) {
		WARN("App custom directory is already exist. remove it!");
		customBaseDir.remove(true);
	}
	customBaseDir.makeDirectory(true);

	runtime::File customCertsDir(this->m_customCertsPath);
	customCertsDir.makeDirectory();

	runtime::File customBundleDir(this->m_customBundlePath);
	customBundleDir.makeDirectory();

	runtime::File appCertsDir(this->m_appCertsPath);
	if (!appCertsDir.exists() || !appCertsDir.isDirectory())
		throw std::invalid_argument("App custom certs path is wrong. : " +
									m_appCertsPath);

	DEBUG("Success to pre-install stage.");
}

int TrustAnchor::Impl::install(bool withSystemCerts) noexcept
{
	EXCEPTION_GUARD_START

	this->preInstall();

	if (withSystemCerts) {
		// link system certificates to the custom directory
		runtime::DirectoryIterator iter(SYS_CERTS_PATH), end;
		while (iter != end) {
			linkTo(readLink(iter->getPath()),
				   this->m_customCertsPath + "/" + iter->getName());
			this->m_customCertNameSet.emplace(iter->getName());
			++iter;
		}
		DEBUG("Success to migrate system certificates.");
	}

	// link app certificates to the custom directory as subjectNameHash
	runtime::DirectoryIterator iter(this->m_appCertsPath), end;
	while (iter != end) {
		Certificate cert(iter->getPath());
		std::string hashName = this->getUniqueHashName(cert.getSubjectNameHash());
		linkTo(iter->getPath(),
			   this->m_customCertsPath + "/" + hashName);
		this->m_customCertNameSet.emplace(std::move(hashName));

		this->m_customCertsData.emplace_back(cert.getCertificateData());
		++iter;
	}

	this->makeCustomBundle(withSystemCerts);

	INFO("Success to install[" << this->m_packageId <<
		 "] to " << this->m_customBasePath);
	return 0;

	EXCEPTION_GUARD_END
}

int TrustAnchor::Impl::uninstall(bool isRollback) noexcept
{
	EXCEPTION_GUARD_START

	runtime::File customBaseDir(this->m_customBasePath);
	if (!customBaseDir.exists() && !isRollback)
		throw std::invalid_argument("There is no installed anchor previous.");

	if (customBaseDir.exists())
		customBaseDir.remove(true);

	INFO("Success to uninstall. : " << this->m_packageId);
	return 0;

	EXCEPTION_GUARD_END
}

bool TrustAnchor::Impl::isSystemCertsModified(void) const
{
	struct stat systemAttr, customAttr;

	stat(SYS_BUNDLE_PATH.c_str(), &systemAttr);
	DEBUG("System bundle mtime : " << ::ctime(&systemAttr.st_mtime));

	auto customBundle = this->m_customBundlePath + "/" + BUNDLE_NAME;
	stat(customBundle.c_str(), &customAttr);
	DEBUG("Custom bundle mtime : " << ::ctime(&customAttr.st_mtime));

	return systemAttr.st_mtime > customAttr.st_mtime;
}

int TrustAnchor::Impl::launch(bool withSystemCerts)
{
	EXCEPTION_GUARD_START

	if (withSystemCerts && this->isSystemCertsModified())
		this->makeCustomBundle(true);

	errno = 0;
	// disassociate from the parent namespace
	if (::unshare(CLONE_NEWNS))
		throw std::logic_error("Failed to unshare namespace > " +
							   std::to_string(errno));

	// convert it to a slave for preventing propagation
	if (::mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL))
		throw std::logic_error("Failed to disconnect root fs.");

	if (::mount(this->m_customCertsPath.c_str(),
				MOUNT_POINT_CERTS.c_str(),
				NULL,
				MS_BIND,
				NULL))
		throw std::logic_error("Failed to mount certs.");

	auto bundle = this->m_customBundlePath + "/" + BUNDLE_NAME;
	if (::mount(bundle.c_str(),
				MOUNT_POINT_BUNDLE.c_str(),
				NULL,
				MS_BIND,
				NULL))
		throw std::logic_error("Failed to mount bundle.");

	INFO("Success to launch. : " << this->m_packageId);
	return 0;

	EXCEPTION_GUARD_END
}

std::string TrustAnchor::Impl::getUniqueHashName(
	const std::string &hashName) const
{
	int sameFileNameCnt = 0;
	std::string uniqueName;
	do {
		uniqueName = hashName + "." + std::to_string(sameFileNameCnt++);
	} while (this->m_customCertNameSet.find(uniqueName) !=
			 this->m_customCertNameSet.end());

	return uniqueName;
}

void TrustAnchor::Impl::makeCustomBundle(bool withSystemCerts)
{
	runtime::File customBundle(this->m_customBundlePath + "/" +
							   BUNDLE_NAME);
	if (customBundle.exists()) {
		WARN("App custom bundle is already exist. remove it!");
		customBundle.remove();
	}

	DEBUG("Start to migrate previous bundle.");
	if (withSystemCerts) {
		runtime::File sysBundle(SYS_BUNDLE_PATH);
		if (!sysBundle.exists())
			throw std::logic_error("There is no system bundle file.");
		sysBundle.copyTo(this->m_customBundlePath);
	} else {
		runtime::File transecBundle(TRANSEC_BUNDLE_PATH);
		if (!transecBundle.exists())
			throw std::logic_error("There is no transec bundle file.");
		transecBundle.copyTo(this->m_customBundlePath);
	}
	DEBUG("Finish migrating previous bundle.");

	if (this->m_customCertsData.empty()) {
		DEBUG("System certificates is changed after TrustAnchor installation.");
		runtime::DirectoryIterator iter(this->m_appCertsPath), end;
		while (iter != end) {
			Certificate cert(iter->getPath());
			this->m_customCertsData.emplace_back(cert.getCertificateData());
			++iter;
		}
	}

	DEBUG("Start to add app's certificate to bundle.");
	customBundle.open(O_RDWR | O_APPEND);
	for (const auto &cert : this->m_customCertsData) {
		customBundle.write(cert.c_str(), cert.length());
		customBundle.write(NEW_LINE.c_str(), NEW_LINE.length());
	}

	INFO("Success to make app custom bundle.");
}

TrustAnchor::TrustAnchor(const std::string &packageId,
						 const std::string &certsDir,
						 uid_t uid) noexcept :
	m_pImpl(new Impl(packageId, certsDir, uid)) {}

TrustAnchor::TrustAnchor(const std::string &packageId,
										   const std::string &certsDir) noexcept :
	m_pImpl(new Impl(packageId, certsDir)) {}

TrustAnchor::~TrustAnchor(void) = default;

int TrustAnchor::install(bool withSystemCerts) noexcept
{
	if (this->m_pImpl == nullptr)
		return -1;

	int ret = this->m_pImpl->install(withSystemCerts);

	if (ret != 0) {
		ERROR("Failed to intall ACTA. Remove custom directory for rollback.");
		this->m_pImpl->uninstall(true);
	}

	return ret;
}

int TrustAnchor::uninstall(void) noexcept
{
	if (this->m_pImpl == nullptr)
		return -1;

	return this->m_pImpl->uninstall();
}

int TrustAnchor::launch(bool withSystemCerts) noexcept
{
	if (this->m_pImpl == nullptr)
		return -1;

	return this->m_pImpl->launch(withSystemCerts);
}

} // namespace transec
