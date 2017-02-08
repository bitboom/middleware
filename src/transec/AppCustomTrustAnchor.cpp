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

#include <set>

#include <klay/filesystem.h>
#include <klay/audit/logger.h>

#include "Certificate.h"
#include "Exception.h"

namespace transec {

class AppCustomTrustAnchor::Impl {
public:
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir,
				  uid_t uid) noexcept;
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir) noexcept;
	virtual ~Impl(void) = default;

	int install(bool withSystemCerts) noexcept;
	int uninstall(void) noexcept;
	int launch(bool withSystemCerts);

private:
	void linkTo(const std::string &src, const std::string &dst);
	void makeCustomBundle(void);
	std::string getHashName(const std::string &filePath);

	std::string m_packageId;
	std::string m_appCertsPath;
	uid_t m_uid;

	std::string m_basePath;
	std::string m_sysCertsPath;
	std::string m_customCertsPath;

	std::set<std::string> m_customCertNameSet;
};

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir,
								 uid_t uid) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_uid(uid),
	m_basePath(std::string(CERTSVC_TRANSEC_DIR)),
	m_sysCertsPath(std::string(TZ_SYS_CA_CERTS)),
	m_customCertsPath(this->m_basePath + "/" +
					  std::to_string(static_cast<int>(uid)) + "/" +
					  packageId) {}

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir) noexcept :
	m_packageId(packageId),
	m_appCertsPath(certsDir),
	m_basePath(std::string(CERTSVC_TRANSEC_DIR)),
	m_sysCertsPath(std::string(TZ_SYS_CA_CERTS)),
	m_customCertsPath(this->m_basePath + "/" + packageId) {}

void AppCustomTrustAnchor::Impl::linkTo(const std::string &src,
										const std::string &dst)
{
	errno = 0;
	int ret = ::symlink(src.c_str(), dst.c_str());
	if (ret != 0)
		throw std::logic_error("Fail to link " + src + " -> " + dst +
							   "[" + std::to_string(errno) + "]");
}

int AppCustomTrustAnchor::Impl::install(bool withSystemCerts) noexcept
{
	EXCEPTION_GUARD_START

	// make the package's custom directory
	runtime::File customDir(this->m_customCertsPath);
	if (customDir.exists()) {
		WARN("App custom certs directory is already exist. remove it!");
		customDir.remove(true);
	}
	customDir.makeDirectory(true);

	// link system certificates to the custom directory
	if (withSystemCerts) {
		runtime::DirectoryIterator iter(this->m_sysCertsPath), end;
		while (iter != end) {
			linkTo(iter->getPath(),
				   this->m_customCertsPath + "/" + iter->getName());
			this->m_customCertNameSet.emplace(iter->getName());
			++iter;
		}
	}

	// link app certificates to the custom directory
	runtime::File appCertsDir(this->m_appCertsPath);
	if (!appCertsDir.exists() || !appCertsDir.isDirectory())
		throw std::invalid_argument("App custom certs path is wrong. : " +
									m_appCertsPath);

	runtime::DirectoryIterator iter(this->m_appCertsPath), end;
	while (iter != end) {
		std::string hashName = this->getHashName(iter->getPath());
		linkTo(iter->getPath(),
			   this->m_customCertsPath + "/" + hashName);
		this->m_customCertNameSet.emplace(std::move(hashName));
		++iter;
	}

	this->makeCustomBundle();

	INFO("Success to install[" << this->m_packageId <<
		 "] to " << this->m_customCertsPath);
	return 0;

	EXCEPTION_GUARD_END
}

int AppCustomTrustAnchor::Impl::uninstall(void) noexcept
{
	EXCEPTION_GUARD_START

	runtime::File customDir(this->m_customCertsPath);
	if (!customDir.exists())
		throw std::invalid_argument("There is no installed directory previous.");

	customDir.remove(true);

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

std::string AppCustomTrustAnchor::Impl::getHashName(const std::string &filePath)
{
	auto hashName = Certificate::getSubjectNameHash(filePath);
	int sameFileNameCnt = 0;
	std::string uniqueName;
	do {
		uniqueName = hashName + "." + std::to_string(sameFileNameCnt++);
	} while (this->m_customCertNameSet.find(uniqueName) != this->m_customCertNameSet.end());

	return uniqueName;
}

void AppCustomTrustAnchor::Impl::makeCustomBundle(void)
{
	// TODO (openssl) make custom bundle file
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
		this->uninstall();
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
