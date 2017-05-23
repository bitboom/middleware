/*
 *  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        trust-anchor.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Implementation of trust anchor
 */
#include "tanchor/trust-anchor.hxx"

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

#include "certificate.hxx"
#include "exception.hxx"

namespace tanchor {

namespace {

const std::string BASE_USR_PATH(TANCHOR_USR_DIR);
const std::string BASE_GLOBAL_PATH(TANCHOR_GLOBAL_DIR);
const std::string TANCHOR_BUNDLE_PATH(TANCHOR_BUNDLE);
const std::string TANCHOR_SYSCA_PATH(TANCHOR_SYSCA);
const std::string SYS_CERTS_PATH(TZ_SYS_CA_CERTS);
const std::string SYS_BUNDLE_PATH(TZ_SYS_CA_BUNDLE);
const std::string MOUNT_POINT_CERTS(TZ_SYS_CA_CERTS);
const std::string MOUNT_POINT_BUNDLE(TZ_SYS_CA_BUNDLE);
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
	int launch(void);

private:
	void preInstall(void) const;
	void preLaunch(void);
	void linkTo(const std::string &src, const std::string &dst) const;
	void makeCustomCerts(bool withSystemCerts);
	void makeCustomBundle(bool withSystemCerts);
	std::string readLink(const std::string &path) const;
	std::string getUniqueHashName(const std::string &hashName) const;
	std::string getFileName(const std::string &path) const;
	bool isSystemCertsModified(void) const;
	void checkFileValidity(const runtime::File &file) const;

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
	ssize_t count = ::readlink(path.c_str(), buf.data(), buf.size());
	return std::string(buf.data(), (count > 0) ? count : 0);
}

void TrustAnchor::Impl::linkTo(const std::string &src,
							   const std::string &dst) const
{
	errno = 0;
	int ret = ::symlink(src.c_str(), dst.c_str());
	if (ret != 0)
		ThrowErrno(errno, "Failed to link " + src + " -> " + dst);
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
	this->checkFileValidity(appCertsDir);
	if (!appCertsDir.isDirectory())
		throw std::invalid_argument("[" + this->m_appCertsPath +
									"] should be directory.");

	DEBUG("Success to pre-install stage.");
}

int TrustAnchor::Impl::install(bool withSystemCerts) noexcept
{
	EXCEPTION_GUARD_START

	this->preInstall();

	this->makeCustomCerts(withSystemCerts);
	this->makeCustomBundle(withSystemCerts);

	INFO("Success to install[" << this->m_packageId <<
		 "] to " << this->m_customBasePath);
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

int TrustAnchor::Impl::uninstall(bool isRollback) noexcept
{
	EXCEPTION_GUARD_START

	runtime::File customBaseDir(this->m_customBasePath);
	if (!customBaseDir.exists() && !isRollback)
		throw std::logic_error("There is no installed anchor previous.");

	if (customBaseDir.exists())
		customBaseDir.remove(true);

	INFO("Success to uninstall. : " << this->m_packageId);
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

void TrustAnchor::Impl::checkFileValidity(const runtime::File &file) const
{
	if (!file.exists())
		ThrowExc(TRUST_ANCHOR_ERROR_NO_SUCH_FILE,
				 "File [" << file.getPath() << "] does not exist.");

	if (!file.canRead())
		ThrowExc(TRUST_ANCHOR_ERROR_PERMISSION_DENIED,
				 "No permission to read [" << file.getPath() << "]");
}

bool TrustAnchor::Impl::isSystemCertsModified(void) const
{
	struct stat systemAttr, customAttr;

	errno = 0;
	if (::stat(SYS_BUNDLE_PATH.c_str(), &systemAttr))
		ThrowErrno(errno, SYS_BUNDLE_PATH);

	auto customBundle = this->m_customBundlePath + "/" +
						this->getFileName(SYS_BUNDLE_PATH);
	if (::stat(customBundle.c_str(), &customAttr))
		ThrowErrno(errno, customBundle);

	DEBUG("System bundle mtime : " << ::ctime(&systemAttr.st_mtime) << ", " <<
		  "Custom bundle mtime : " << ::ctime(&customAttr.st_mtime));

	return systemAttr.st_mtime > customAttr.st_mtime;
}

void TrustAnchor::Impl::preLaunch(void)
{
	// check whether system certificates use or not
	runtime::File customSysCA(this->m_customBasePath + "/" +
							  this->getFileName(TANCHOR_SYSCA_PATH));
	if (!customSysCA.exists()) {
		INFO("This package only use custom certificates.");
		return;
	}

	INFO("This package use system certificates.");
	if (this->isSystemCertsModified()) {
		WARN("System certificates be changed. Remake custom bundle.");
		this->makeCustomBundle(true);
	}

	DEBUG("Success to pre-install stage.");
}

int TrustAnchor::Impl::launch()
{
	EXCEPTION_GUARD_START

	this->preLaunch();

	errno = 0;
	// disassociate from the parent namespace
	if (::unshare(CLONE_NEWNS))
		ThrowErrno(errno, "Failed to unshare.");

	// convert it to a slave for preventing propagation
	if (::mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL))
		ThrowErrno(errno, "Failed to mount.");

	if (::mount(this->m_customCertsPath.c_str(),
				MOUNT_POINT_CERTS.c_str(),
				NULL,
				MS_BIND,
				NULL))
		ThrowErrno(errno, "Failed to mount. src[" +
						  this->m_customCertsPath + "] to dst[" +
						  MOUNT_POINT_CERTS + "]");

	auto bundle = this->m_customBundlePath + "/" +
				  this->getFileName(SYS_BUNDLE_PATH);
	if (::mount(bundle.c_str(),
				MOUNT_POINT_BUNDLE.c_str(),
				NULL,
				MS_BIND,
				NULL))
		ThrowErrno(errno, "Failed to mount. src[" + bundle +
						  "] to dst[" + MOUNT_POINT_BUNDLE + "]");

	INFO("Success to launch. : " << this->m_packageId);
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

std::string TrustAnchor::Impl::getFileName(const std::string &path) const
{
	size_t pos = path.rfind('/');
	if (pos == std::string::npos)
		throw std::logic_error("Path is wrong. > " + path);

	return path.substr(pos + 1);
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

void TrustAnchor::Impl::makeCustomCerts(bool withSystemCerts)
{
	if (withSystemCerts) {
		// link system certificates to the custom directory
		runtime::DirectoryIterator iter(SYS_CERTS_PATH), end;
		while (iter != end) {
			linkTo(this->readLink(iter->getPath()),
				   this->m_customCertsPath + "/" + iter->getName());
			this->m_customCertNameSet.emplace(iter->getName());
			++iter;
		}
		DEBUG("Success to migrate system certificates.");

		// copy sysca(withSystemCerts flag) and check at launching time
		runtime::File tanchorSysCA(TANCHOR_SYSCA_PATH);
		this->checkFileValidity(tanchorSysCA);
		tanchorSysCA.copyTo(this->m_customBasePath);
		DEBUG("Success to set SYSCA flag.");
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
}

void TrustAnchor::Impl::makeCustomBundle(bool withSystemCerts)
{
	runtime::File customBundle(this->m_customBundlePath + "/" +
							   this->getFileName(SYS_BUNDLE_PATH));
	if (customBundle.exists()) {
		WARN("App custom bundle is already exist. remove it!");
		customBundle.remove();
	}

	DEBUG("Start to migrate previous bundle.");
	if (withSystemCerts) {
		runtime::File sysBundle(SYS_BUNDLE_PATH);
		this->checkFileValidity(sysBundle);
		sysBundle.copyTo(this->m_customBundlePath);
	} else {
		runtime::File tanchorBundle(TANCHOR_BUNDLE_PATH);
		this->checkFileValidity(tanchorBundle);
		tanchorBundle.copyTo(this->m_customBundlePath);
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
		return TRUST_ANCHOR_ERROR_OUT_OF_MEMORY;

	int ret = this->m_pImpl->install(withSystemCerts);

	if (ret != TRUST_ANCHOR_ERROR_NONE) {
		ERROR("Failed to intall ACTA. Remove custom directory for rollback.");
		this->m_pImpl->uninstall(true);
	}

	return ret;
}

int TrustAnchor::uninstall(void) noexcept
{
	if (this->m_pImpl == nullptr)
		return TRUST_ANCHOR_ERROR_OUT_OF_MEMORY;

	return this->m_pImpl->uninstall();
}

int TrustAnchor::launch(void) noexcept
{
	if (this->m_pImpl == nullptr)
		return TRUST_ANCHOR_ERROR_OUT_OF_MEMORY;

	return this->m_pImpl->launch();
}

} // namespace tanchor
