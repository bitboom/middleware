#  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License

SET(VERSION "0.1.0")
SET(OSQUERY_VERSION "4.0.0")

SET(RO_DIR "/usr/share")
SET(RW_DIR "/opt/data")

SET(CMAKE_INSTALL_PREFIX /usr)

SET(VIST_RO_DIR "${RO_DIR}/vist")
SET(VIST_RW_DIR "${RW_DIR}/vist")

SET(VIST_DB_DIR     "${VIST_RW_DIR}/db")
SET(VIST_PLUGIN_DIR "${VIST_RO_DIR}/plugin")
SET(VIST_SCRIPT_DIR "${VIST_RO_DIR}/script")

SET(DEFAULT_POLICY_ADMIN "vist-cli")
SET(DB_INSTALL_DIR     "${VIST_DB_DIR}")
SET(PLUGIN_INSTALL_DIR "${VIST_PLUGIN_DIR}")
SET(SCRIPT_INSTALL_DIR "${VIST_SCRIPT_DIR}")

EXECUTE_PROCESS(COMMAND mkdir -p "${VIST_DB_DIR}")
EXECUTE_PROCESS(COMMAND mkdir -p "${VIST_PLUGIN_DIR}")
EXECUTE_PROCESS(COMMAND mkdir -p "${VIST_SCRIPT_DIR}")
