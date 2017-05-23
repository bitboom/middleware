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
 * @file        launcher.c
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     0.1
 * @brief       Launcher API example
 */

#include <tanchor/trust-anchor.h>
#include <tanchor/error.h>

#include <stdio.h>

int main()
{
	/*
	 * When app launched trust_anchor_global_launch() should be called once.
	 * If app is user-app, call trust_anchor_usr_launch() with uid.
	 *
	 * [caution]
	 * Since trust anchor launch operation disassocaite namespace,
	 * other mount tasks should be done before trust_anchor_usr_launch() called.
	 *
	 * [pre-condition]
	 * 1. Launcher should have CAP_SYS_ADMIN.
	 */
	int ret = trust_anchor_global_launch("pkgid", "/app_certs_path");
	if (ret != TRUST_ANCHOR_ERROR_NONE) {
		printf("Failed to launch operation");
		return -1;
	}

	return 0;
}
