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
 * @file        Api.cpp
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Implementation of App custom trust anchor CAPI.
 */
#include "acta/app-custom-trust-anchor.h"

#include "AppCustomTrustAnchor.h"

using namespace transec;

int acta_global_install(const char *package_id,
						const char *app_certificates_path,
						bool with_system_certificates)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path);
	return acta.install(with_system_certificates);
}

int acta_usr_install(const char *package_id,
					 const char *app_certificates_path,
					 uid_t uid,
					 bool with_system_certificates)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path, uid);
	return acta.install(with_system_certificates);
}

int acta_global_launch(const char *package_id,
					   const char *app_certificates_path,
					   bool with_system_certificates)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path);
	return acta.launch(with_system_certificates);
}

int acta_usr_launch(const char *package_id,
					const char *app_certificates_path,
					uid_t uid,
					bool with_system_certificates)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path, uid);
	return acta.launch(with_system_certificates);
}

int acta_global_uninstall(const char *package_id,
						  const char *app_certificates_path)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path);
	return acta.uninstall();
}

int acta_usr_uninstall(const char *package_id,
					   const char *app_certificates_path,
					   uid_t uid)
{
	AppCustomTrustAnchor acta(package_id, app_certificates_path, uid);
	return acta.uninstall();
}
