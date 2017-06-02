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

#include <klay/audit/logger.h>

#include "logic.hxx"
#include "exception.hxx"
#include "environment.hxx"

namespace tanchor {

class TrustAnchor::Impl {
public:
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir,
				  uid_t uid) noexcept;
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir) noexcept;
	virtual ~Impl(void) = default;

	int install(bool withSystemCerts) noexcept;
	int uninstall(void) const noexcept;
	int launch(void) noexcept;
	int rollback(void) const noexcept;

private:
	void preInstall(void);
	void preLaunch(void);

	Logic m_logic;
};

TrustAnchor::Impl::Impl(const std::string &packageId,
						const std::string &certsDir,
						uid_t uid) noexcept :
	m_logic(certsDir,
			path::BASE_USR_PATH + "/" +
			std::to_string(static_cast<int>(uid)) + "/" +
			packageId)
{
	INFO("Start tanchor about usr[" << uid << "], app[" << packageId << "]");
}

TrustAnchor::Impl::Impl(const std::string &packageId,
						const std::string &certsDir) noexcept :
	m_logic(certsDir, path::BASE_GLOBAL_PATH + "/" + packageId)
{
	INFO("Start tanchor about global app[" << packageId << "]");
}

void TrustAnchor::Impl::preInstall(void)
{
	this->m_logic.init();

	if (!this->m_logic.isAppCertsValid())
		throw std::invalid_argument("App certs dir should be directory.");
	DEBUG("Success to pre-install stage.");
}

int TrustAnchor::Impl::install(bool withSystemCerts) noexcept
{
	EXCEPTION_GUARD_START

	this->preInstall();

	if (withSystemCerts)
		this->m_logic.setSystemCertsUsed();

	this->m_logic.makeCustomCerts();
	this->m_logic.makeCustomBundle();

	INFO("Success to install.");
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

int TrustAnchor::Impl::rollback(void) const noexcept
{
	EXCEPTION_GUARD_START

	this->m_logic.deinit(true);

	INFO("Success to rollback.");
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

int TrustAnchor::Impl::uninstall(void) const noexcept
{
	EXCEPTION_GUARD_START

	this->m_logic.deinit(false);

	INFO("Success to uninstall.");
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
}

void TrustAnchor::Impl::preLaunch(void)
{
	if (!this->m_logic.isSystemCertsUsed())
		return;

	DEBUG("This package use system certificates.");
	if (this->m_logic.isSystemCertsModified()) {
		WARN("System certificates be changed. Do re-install for refresh.");
		this->install(true);
	}
	DEBUG("Success to pre-launch stage.");
}

int TrustAnchor::Impl::launch() noexcept
{
	EXCEPTION_GUARD_START

	this->preLaunch();

	this->m_logic.disassociateNS();
	this->m_logic.mountCustomCerts();
	this->m_logic.mountCustomBundle();

	INFO("Success to launch.");
	return TRUST_ANCHOR_ERROR_NONE;

	EXCEPTION_GUARD_END
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
		if (this->m_pImpl->rollback() != TRUST_ANCHOR_ERROR_NONE)
			ERROR("Failed to rollback ACTA.");
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
