# Copyright (c) 2011-2015 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author
# @brief
#
############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("auth-fw")

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)

############################# compiler flags ##################################

SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -std=c++0x -O0 -pg -Wp,-U_FORTIFY_SOURCE -fno-rtti")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb -Wp,-U_FORTIFY_SOURCE -fno-rtti")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2 -fno-rtti")
SET(CMAKE_C_FLAGS_CCOV         "-g -O2 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "-g -std=c++0x -O2 --coverage -fno-rtti")

# If supported for the target machine, emit position-independent code,suitable
# for dynamic linking and avoiding any limit on the size of the global offset
# table. This option makes a difference on the m68k, PowerPC and SPARC.
# (BJ: our ARM too?)
ADD_DEFINITIONS("-fPIC")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

STRING(REGEX MATCH "([^.]*)" API_VERSION "${VERSION}")
ADD_DEFINITIONS("-DSMACK_ENABLED")
ADD_DEFINITIONS("-DSERVICE_NAME=\"${SERVICE_NAME}\"")
ADD_DEFINITIONS("-DVERSION=${VERSION}")
ADD_DEFINITIONS("-DBIN_DIR=\"${BIN_DIR}\"")
ADD_DEFINITIONS("-DRUN_DIR=\"${RUN_DIR}\"")
ADD_DEFINITIONS("-DRW_DATA_DIR=\"${RW_DATA_DIR}\"")
ADD_DEFINITIONS("-DSOCK_PASSWD_CHECK=\"${SOCK_PASSWD_CHECK}\"")
ADD_DEFINITIONS("-DSOCK_PASSWD_SET=\"${SOCK_PASSWD_SET}\"")
ADD_DEFINITIONS("-DSOCK_PASSWD_RESET=\"${SOCK_PASSWD_RESET}\"")
ADD_DEFINITIONS("-DSOCK_PASSWD_POLICY=\"${SOCK_PASSWD_POLICY}\"")
ADD_DEFINITIONS("-DPLUGIN_SW_BACKEND_PATH=\"${PLUGIN_SW_BACKEND_PATH}\"")
ADD_DEFINITIONS("-DPLUGIN_TZ_BACKEND_PATH=\"${PLUGIN_TZ_BACKEND_PATH}\"")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
    ADD_DEFINITIONS("-DDLOG_DEBUG_ENABLED")
    ADD_DEFINITIONS("-DDPL_LOGS_ENABLED")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

SET(INCLUDE_PATH ${PROJECT_SOURCE_DIR}/src/include)
SET(COMMON_PATH  ${PROJECT_SOURCE_DIR}/src/common)
SET(CLIENT_PATH  ${PROJECT_SOURCE_DIR}/src/client)
SET(SERVER_PATH  ${PROJECT_SOURCE_DIR}/src/server)
SET(PLUGIN_PATH  ${PROJECT_SOURCE_DIR}/src/server/plugin)
SET(DPL_PATH     ${PROJECT_SOURCE_DIR}/src/dpl)

SET(TARGET_SERVER ${SERVICE_NAME})
SET(TARGET_CLIENT ${SERVICE_NAME}-client)
SET(TARGET_CLIENT_ADMIN ${SERVICE_NAME}-client-admin)
SET(TARGET_COMMON ${SERVICE_NAME}-commons)
SET(TARGET_SW_BACKEND ${SERVICE_NAME}-sw-backend)
SET(TARGET_TZ_BACKEND ${SERVICE_NAME}-tz-backend)
SET(TARGET_TEST ${SERVICE_NAME}-test)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(pkgconfig)
ADD_SUBDIRECTORY(systemd)
ADD_SUBDIRECTORY(conf)
ADD_SUBDIRECTORY(tests)
