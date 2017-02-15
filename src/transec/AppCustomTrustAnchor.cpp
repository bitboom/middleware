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
 * @file        AppCustomTrustAnchor.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Implementation of App custom trust anchor
 */
#include "AppCustomTrustAnchor.h"

#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

#include <set>
#include <vector>

#include <klay/filesystem.h>
#include <klay/audit/logger.h>

#include "Certificate.h"
#include "Exception.h"

namespace transec {

namespace {

const std::string BASE_PATH(CERTSVC_TRANSEC_DIR);
const std::string BASE_CERTS_PATH(BASE_PATH + "/certs");
const std::string BASE_BUNDLE_PATH(BASE_PATH + "/bundle");
const std::string SYS_CERTS_PATH(TZ_SYS_CA_CERTS);
const std::string SYS_BUNDLE_PATH(TZ_SYS_CA_BUNDLE);
const std::string BUNDLE_NAME("ca-bundle.pem");
const std::string NEW_LINE("\n");

} // namespace anonymous

class AppCustomTrustAnchor::Impl {
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
	void makeCustomBundle(const std::vector<std::string> &certs) const;
	std::string getUniqueHashName(const std::string &hashName) const;

	std::string m_packageId;
	std::string m_appCertsPath;
	uid_t m_uid;

	std::string m_customCertsPath;
	std::string m_customBundlePath;

	std::set<std::string> m_customCertNameSet;
};

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir,
								 uid_t uid) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_uid(uid),
	m_customCertsPath(BASE_CERTS_PATH + "/usr/" +
					  std::to_string(static_cast<int>(uid)) + "/" +
					  packageId),
	m_customBundlePath(BASE_BUNDLE_PATH + "/usr/" +
					   std::to_string(static_cast<int>(uid)) + "/" +
					   packageId),
	m_customCertNameSet() {}

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_uid(-1),
	m_customCertsPath(BASE_CERTS_PATH + "/global/" + packageId),
	m_customBundlePath(BASE_BUNDLE_PATH + "/global/" + packageId),
	m_customCertNameSet() {}

void AppCustomTrustAnchor::Impl::linkTo(const std::string &src,
										const std::string &dst) const
{
	errno = 0;
	int ret = ::symlink(src.c_str(), dst.c_str());
	if (ret != 0)
		throw std::logic_error("Fail to link " + src + " -> " + dst +
							   "[" + std::to_string(errno) + "]");
}

void AppCustomTrustAnchor::Impl::preInstall(void) const
{
	runtime::File customCertsDir(this->m_customCertsPath);
	if (customCertsDir.exists()) {
		WARN("App custom certs directory is already exist. remove it!");
		customCertsDir.remove(true);
	}
	customCertsDir.makeDirectory(true);

	runtime::File customBundleDir(this->m_customBundlePath);
	if (customBundleDir.exists()) {
		WARN("App custom bundle directory is already exist. remove it!");
		customBundleDir.remove(true);
	}
	customBundleDir.makeDirectory(true);

	runtime::File appCertsDir(this->m_appCertsPath);
	if (!appCertsDir.exists() || !appCertsDir.isDirectory())
		throw std::invalid_argument("App custom certs path is wrong. : " +
									m_appCertsPath);

	DEBUG("Success to pre-install stage.");
}

int AppCustomTrustAnchor::Impl::install(bool withSystemCerts) noexcept
{
	EXCEPTION_GUARD_START

	this->preInstall();

	if (withSystemCerts) {
		// link system certificates to the custom directory
		runtime::DirectoryIterator iter(SYS_CERTS_PATH), end;
		while (iter != end) {
			linkTo(iter->getPath(),
				   this->m_customCertsPath + "/" + iter->getName());
			this->m_customCertNameSet.emplace(iter->getName());
			++iter;
		}

		// copy system bundle to the custom path
		runtime::File sysBundle(SYS_BUNDLE_PATH);
		if (!sysBundle.exists())
			throw std::logic_error("There is no system bundle file.");
		sysBundle.copyTo(this->m_customBundlePath);

		DEBUG("Success to migrate system certificates and bundle.");
	}

	// link app certificates to the custom directory as subjectNameHash
	runtime::DirectoryIterator iter(this->m_appCertsPath), end;
	std::vector<std::string> customCertData;
	while (iter != end) {
		Certificate cert(iter->getPath());
		std::string hashName = this->getUniqueHashName(cert.getSubjectNameHash());
		linkTo(iter->getPath(),
			   this->m_customCertsPath + "/" + hashName);
		this->m_customCertNameSet.emplace(std::move(hashName));

		customCertData.emplace_back(cert.getCertificateData());
		++iter;
	}

	this->makeCustomBundle(customCertData);

	INFO("Success to install[" << this->m_packageId <<
		 "] to " << this->m_customCertsPath);
	return 0;

	EXCEPTION_GUARD_END
}

int AppCustomTrustAnchor::Impl::uninstall(bool isRollback) noexcept
{
	EXCEPTION_GUARD_START

	runtime::File customCertsDir(this->m_customCertsPath);
	if (!customCertsDir.exists() && !isRollback)
		throw std::invalid_argument("There is no installed certs previous.");

	runtime::File customBundleDir(this->m_customBundlePath);
	if (!customBundleDir.exists() && !isRollback)
		throw std::invalid_argument("There is no installed bundle previous.");

	if (customCertsDir.exists())
		customCertsDir.remove(true);

	if (!customBundleDir.exists())
		customBundleDir.remove(true);

	INFO("Success to uninstall. : " << this->m_packageId);
	return 0;

	EXCEPTION_GUARD_END
}

int AppCustomTrustAnchor::Impl::launch(bool withSystemCerts)
{
	if (withSystemCerts)
		return 0;
	else
		return -1;
}

std::string AppCustomTrustAnchor::Impl::getUniqueHashName(
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

void AppCustomTrustAnchor::Impl::makeCustomBundle(
	const std::vector<std::string> &certs) const
{
	runtime::File customBundle(this->m_customBundlePath + "/" +
							   BUNDLE_NAME);
	if (!customBundle.exists()) {
		DEBUG("Make bundle only used by app certificates.");
		customBundle.create(755);
	}

	customBundle.open(O_RDWR | O_APPEND);
	for (const auto &cert : certs) {
		customBundle.write(cert.c_str(), cert.length());
		customBundle.write(NEW_LINE.c_str(), NEW_LINE.length());
	}
}

AppCustomTrustAnchor::AppCustomTrustAnchor(const std::string &packageId,
										   const std::string &certsDir,
										   uid_t uid) noexcept :
	m_pImpl(new Impl(packageId, certsDir, uid)) {}

AppCustomTrustAnchor::AppCustomTrustAnchor(const std::string &packageId,
										   const std::string &certsDir) noexcept :
	m_pImpl(new Impl(packageId, certsDir)) {}

AppCustomTrustAnchor::~AppCustomTrustAnchor(void) = default;

int AppCustomTrustAnchor::install(bool withSystemCerts) noexcept
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

int AppCustomTrustAnchor::uninstall(void) noexcept
{
	if (this->m_pImpl == nullptr)
		return -1;

	return this->m_pImpl->uninstall();
}

int AppCustomTrustAnchor::launch(bool withSystemCerts) noexcept
{
	if (this->m_pImpl == nullptr)
		return -1;

	return this->m_pImpl->launch(withSystemCerts);
}

} // namespace transec
