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
 * @file        environment.hxx
 * @author      Sangwan Kwon (sangwan.kwon@samsung.com)
 * @version     1.0
 * @brief       Environment variables
 */
#pragma once

namespace tanchor {
namespace path {

const std::string BASE_USR_PATH(TANCHOR_USR_DIR);
const std::string BASE_GLOBAL_PATH(TANCHOR_GLOBAL_DIR);
const std::string SYS_CERTS_PATH(TZ_SYS_CA_CERTS);
const std::string SYS_BUNDLE_PATH(TZ_SYS_CA_BUNDLE);

const std::string TANCHOR_SYSCA_PATH = TANCHOR_SYSCA;
const std::string TANCHOR_BUNDLE_PATH = TANCHOR_BUNDLE;

const std::string MOUNT_POINT_CERTS(TZ_SYS_CA_CERTS);
const std::string MOUNT_POINT_BUNDLE(TZ_SYS_CA_BUNDLE);

} // namespace path
} // namespace tanchor
