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
 * @file        logic.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief
 */
#include "logic.hxx"

#include <ctime>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include <klay/filesystem.h>

#include "logger.hxx"
#include "certificate.hxx"
#include "file-system.hxx"
#include "exception.hxx"
#include "environment.hxx"

namespace tanchor {

Logic::Logic(const std::string &basePath) :
	m_customBasePath(basePath),
	m_customCertsPath(m_customBasePath + "/certs"),
	m_customBundlePath(m_customBasePath + "/bundle"),
	m_customCertNameSet(),
	m_customCertsData() {}

void Logic::init(void) const
{
	runtime::File customBaseDir(this->m_customBasePath);
	if (customBaseDir.exists()) {
		WARN(SINK, "Pkg custom directory is already exist. remove it!");
		customBaseDir.remove(true);
	}
	customBaseDir.makeDirectory(true);

	runtime::File customCertsDir(this->m_customCertsPath);
	customCertsDir.makeDirectory();

	runtime::File customBundleDir(this->m_customBundlePath);
	customBundleDir.makeDirectory();
	DEBUG(SINK, "Success to init[" << this->m_customBasePath << "]");
}

void Logic::deinit(bool isRollback) const
{
	runtime::File customBaseDir(this->m_customBasePath);
	if (customBaseDir.exists())
		customBaseDir.remove(true);
	else if (!isRollback)
		WARN(SINK, "tanchor is never installed before.");

	DEBUG(SINK, "Success to deinit[" << this->m_customBasePath << "]");
}

void Logic::makeCustomCerts(void)
{
	if (this->isSystemCertsUsed()) {
		// link system certificates to the custom directory
		runtime::DirectoryIterator iter(path::SYS_CERTS_PATH), end;
		while (iter != end) {
			auto name = iter->getName();
			File::linkTo(File::readLink(iter->getPath()),
						 this->m_customCertsPath + "/" + name);
			this->m_customCertNameSet.emplace(File::getName(iter->getPath()));
			++iter;
		}
		DEBUG(SINK, "Success to migrate system certificates.");
	}

	// link pkg certificates to the custom directory as subjectNameHash
	runtime::DirectoryIterator iter(this->getPkgCertsPath()), end;
	while (iter != end) {
		Certificate cert(iter->getPath());
		std::string uName = this->getUniqueCertName(cert.getSubjectNameHash());
		File::linkTo(iter->getPath(), this->m_customCertsPath + "/" + uName);
		this->m_customCertNameSet.emplace(std::move(uName));

		this->m_customCertsData.emplace_back(cert.getCertificateData());
		++iter;
	}

	INFO(SINK, "Success to make pkg custom certs.");
}

void Logic::makeCustomBundle(void)
{
	runtime::File customBundle(this->m_customBundlePath + "/" +
							   File::getName(path::SYS_BUNDLE_PATH));
	if (customBundle.exists()) {
		WARN(SINK, "Pkg custom bundle is already exist. remove it!");
		customBundle.remove();
	}

	DEBUG(SINK, "Start to migrate previous bundle.");
	if (this->isSystemCertsUsed()) {
		runtime::File sysBundle(path::SYS_BUNDLE_PATH);
		sysBundle.copyTo(this->m_customBundlePath);
	} else {
		runtime::File tanchorBundle(path::TANCHOR_BUNDLE_PATH);
		tanchorBundle.copyTo(this->m_customBundlePath);
	}
	DEBUG(SINK, "Finish migrating previous bundle.");

	if (this->m_customCertsData.empty()) {
		DEBUG(SINK, "System certificates is changed after TrustAnchor installation.");
		runtime::DirectoryIterator iter(this->getPkgCertsPath()), end;
		while (iter != end) {
			Certificate cert(iter->getPath());
			this->m_customCertsData.emplace_back(cert.getCertificateData());
			++iter;
		}
	}

	DEBUG(SINK, "Start to add pkg's certificate to bundle.");
	customBundle.open(O_RDWR | O_APPEND);
	for (const auto &cert : this->m_customCertsData) {
		customBundle.write(cert.c_str(), cert.length());
		std::string newLine = "\n";
		customBundle.write(newLine.c_str(), newLine.length());
	}

	INFO(SINK, "Success to make pkg custom bundle.");
}

bool Logic::isCustomBaseValid(void) const
{
	runtime::File customBaseDir(this->m_customBasePath);
	return customBaseDir.exists();
}

bool Logic::isPkgCertsValid(const std::string &path) const
{
	runtime::File file(path);

	if (!file.exists())
		ThrowExc(TRUST_ANCHOR_ERROR_NO_SUCH_FILE,
				 "File [" << file.getPath() << "] does not exist.");

	if (!file.canRead())
		ThrowExc(TRUST_ANCHOR_ERROR_PERMISSION_DENIED,
				 "No permission to read [" << file.getPath() << "]");

	return file.isDirectory();
}

void Logic::setPkgCertsPath(const std::string &path) const
{
	if (!this->isPkgCertsValid(path))
		throw std::invalid_argument("Pkg certs dir should be directory.");


	runtime::File tanchorPkgCerts(path::TANCHOR_PKG_CERTS_PATH);
	tanchorPkgCerts.copyTo(this->m_customBasePath);

	runtime::File pkgCertsPath(this->m_customBasePath + "/" +
							   File::getName(path::TANCHOR_PKG_CERTS_PATH));
	pkgCertsPath.open(O_RDWR | O_APPEND);
	pkgCertsPath.lock();
	pkgCertsPath.write(path.c_str(), path.length());
	pkgCertsPath.unlock();

	DEBUG(SINK, "Success to set package certificates path : " << path);
}

std::string Logic::getPkgCertsPath(void) const
{
	std::string path = File::read(this->m_customBasePath + "/" +
								  File::getName(path::TANCHOR_PKG_CERTS_PATH));
	DEBUG(SINK, "Package certificates path : " << path);
	return path;
}

void Logic::setSystemCertsUsed(void) const
{
	runtime::File tanchorSysCA(path::TANCHOR_SYSCA_PATH);
	tanchorSysCA.copyTo(this->m_customBasePath);
	DEBUG(SINK, "Success to set SYSCA flag.");
}

bool Logic::isSystemCertsUsed(void) const
{
	runtime::File customSysCA(this->m_customBasePath + "/" +
							  File::getName(path::TANCHOR_SYSCA_PATH));

	DEBUG(SINK, "Wheter system certificates use or not : " << customSysCA.exists());
	return customSysCA.exists();
}

bool Logic::isSystemCertsModified(void) const
{
	struct stat systemAttr, customAttr;
	auto customBundle = this->m_customBundlePath + "/" +
						File::getName(path::SYS_BUNDLE_PATH);

	if (::stat(path::SYS_BUNDLE_PATH.c_str(), &systemAttr))
		ThrowErrno(errno, path::SYS_BUNDLE_PATH);

	if (::stat(customBundle.c_str(), &customAttr))
		ThrowErrno(errno, customBundle);

	DEBUG(SINK, "System bundle mtime : " << ::ctime(&systemAttr.st_mtime) << ", " <<
		  "Custom bundle mtime : " << ::ctime(&customAttr.st_mtime));

	return systemAttr.st_mtime > customAttr.st_mtime;
}

void Logic::disassociateNS(void) const
{
	if (::unshare(CLONE_NEWNS))
		ThrowErrno(errno, "Failed to unshare.");

	// convert it to a slave for preventing propagation
	if (::mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL))
		ThrowErrno(errno, "Failed to mount.");

	DEBUG(SINK, "Success to disassociate namespace.");
}

void Logic::mountCustomCerts(void) const
{
	if (::mount(this->m_customCertsPath.c_str(),
				path::MOUNT_POINT_CERTS.c_str(),
				NULL,
				MS_BIND,
				NULL))
		ThrowErrno(errno, "Failed to mount. src[" +
						  this->m_customCertsPath + "] to dst[" +
						  path::MOUNT_POINT_CERTS + "]");
}

void Logic::mountCustomBundle(void) const
{
	auto bundle = this->m_customBundlePath + "/" +
				  File::getName(path::SYS_BUNDLE_PATH);
	if (::mount(bundle.c_str(),
				path::MOUNT_POINT_BUNDLE.c_str(),
				NULL,
				MS_BIND,
				NULL))
		ThrowErrno(errno, "Failed to mount. src[" + bundle +
						  "] to dst[" + path::MOUNT_POINT_BUNDLE + "]");
}

std::string Logic::getUniqueCertName(const std::string &name) const
{
	int sameNameCnt = 0;
	std::string uName;
	do {
		uName = name + "." + std::to_string(sameNameCnt++);
	} while (this->m_customCertNameSet.find(uName) !=
			 this->m_customCertNameSet.end());

	return uName;
}

} // namespace tanchor
