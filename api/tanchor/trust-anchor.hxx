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


/**
 * @file trust-anchor.hxx
 * @brief Trust anchor C++ API header
 */


#pragma once


#include <sys/types.h>
#include <string>
#include <memory>


#define TANCHOR_API __attribute__((visibility("default")))


namespace tanchor {

class TANCHOR_API TrustAnchor {
public:
	explicit TrustAnchor(const std::string &packageId,
						 const std::string &certsDir,
						 uid_t uid) noexcept;
	explicit TrustAnchor(const std::string &packageId,
						 const std::string &certsDir) noexcept;
	virtual ~TrustAnchor(void);

	TrustAnchor(const TrustAnchor &) = delete;
	TrustAnchor(TrustAnchor &&) = delete;
	TrustAnchor &operator=(const TrustAnchor &) = delete;
	TrustAnchor &operator=(TrustAnchor &&) = delete;

	int install(bool withSystemCerts) noexcept;
	int uninstall(void) noexcept;
	int launch(bool withSystemCerts) noexcept;

private:
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

} // namespace tanchor
