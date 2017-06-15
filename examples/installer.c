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
 * @file        installer.c
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Installer API example
 */

#include <tanchor/trust-anchor.h>
#include <tanchor/error.h>

#include <stdio.h>

int main()
{
	/*
	 * When app installed trust_anchor_global_install() should be called once.
	 * If app is user-app, call trust_anchor_usr_install() with uid.
	 *
	 * [pre-condition]
	 * 1. Get with_sys_certs information from App configuration.
	 */
	bool with_sys = false;

	int ret = trust_anchor_global_install("pkgid", "/app_certs_path", with_sys);
	if (ret != TRUST_ANCHOR_ERROR_NONE) {
		printf("Failed to install operation");
		return -1;
	}

	/*
	 * When app uninstalled trust_anchor_global_uninstall() should be called once.
	 * If app is user-app, call trust_anchor_usr_uninstall() with uid.
	 */
	ret = trust_anchor_global_uninstall("pkgid", "/app_certs_path");
	if (ret != TRUST_ANCHOR_ERROR_NONE) {
		printf("Failed to uninstall operation");
		return -1;
	}

	return 0;
}
