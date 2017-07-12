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
 * @file        api.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Implementation of trust anchor CAPI
 */
#include "tanchor/trust-anchor.h"
#include "tanchor/trust-anchor.hxx"

#include "macros.hxx"

using namespace tanchor;

TANCHOR_API
int trust_anchor_install(const char *package_id,
						 const char *pkg_certs_path,
						 uid_t uid,
						 bool with_system_certs)
{
	TrustAnchor ta(package_id, pkg_certs_path, uid);
	return ta.install(with_system_certs);
}

TANCHOR_API
int trust_anchor_launch(const char *package_id,
						const char *pkg_certs_path,
						uid_t uid)
{
	TrustAnchor ta(package_id, pkg_certs_path, uid);
	return ta.launch();
}

TANCHOR_API
int trust_anchor_uninstall(const char *package_id,
						   const char *pkg_certs_path,
						   uid_t uid)
{
	TrustAnchor ta(package_id, pkg_certs_path, uid);
	return ta.uninstall();
}
