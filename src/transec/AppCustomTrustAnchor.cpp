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

namespace transec {

class AppCustomTrustAnchor::Impl {
public:
	explicit Impl(const std::string &packageId,
				  const std::string &certsDir,
				  uid_t uid);
	explicit Impl(const std::string &packageId, const std::string &certsDir);
	virtual ~Impl(void) = default;

	int install(bool withSystemCerts);
	int uninstall(void);
	int launch(bool withSystemCerts);

private:
	std::string m_packageId;
	std::string m_certsDir;
	uid_t m_uid;
};

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir,
								 uid_t uid) :
	m_packageId(packageId), m_certsDir(certsDir), m_uid(uid) {}

AppCustomTrustAnchor::Impl::Impl(const std::string &packageId,
								 const std::string &certsDir) :
	m_packageId(packageId), m_certsDir(certsDir) {}

int AppCustomTrustAnchor::Impl::install(bool withSystemCerts)
{
	if (withSystemCerts)
		return 0;
	else
		return -1;
}

int AppCustomTrustAnchor::Impl::uninstall(void)
{
	return 0;
}

int AppCustomTrustAnchor::Impl::launch(bool withSystemCerts)
{
	if (withSystemCerts)
		return 0;
	else
		return -1;
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

	return this->m_pImpl->install(withSystemCerts);
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
