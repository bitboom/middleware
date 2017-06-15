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
int trust_anchor_global_install(const char *package_id,
								const char *app_certificates_path,
								bool with_system_certificates)
{
	TrustAnchor ta(package_id, app_certificates_path);
	return ta.install(with_system_certificates);
}

TANCHOR_API
int trust_anchor_usr_install(const char *package_id,
							 const char *app_certificates_path,
							 uid_t uid,
							 bool with_system_certificates)
{
	TrustAnchor ta(package_id, app_certificates_path, uid);
	return ta.install(with_system_certificates);
}

TANCHOR_API
int trust_anchor_global_launch(const char *package_id,
							   const char *app_certificates_path)
{
	TrustAnchor ta(package_id, app_certificates_path);
	return ta.launch();
}

TANCHOR_API
int trust_anchor_usr_launch(const char *package_id,
							const char *app_certificates_path,
							uid_t uid)
{
	TrustAnchor ta(package_id, app_certificates_path, uid);
	return ta.launch();
}

TANCHOR_API
int trust_anchor_global_uninstall(const char *package_id,
								  const char *app_certificates_path)
{
	TrustAnchor ta(package_id, app_certificates_path);
	return ta.uninstall();
}

TANCHOR_API
int trust_anchor_usr_uninstall(const char *package_id,
							   const char *app_certificates_path,
							   uid_t uid)
{
	TrustAnchor ta(package_id, app_certificates_path, uid);
	return ta.uninstall();
}
