#  Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
#
# @file      CMakeLists.txt
# @author    Sangwan kwon (sangwan.kwon@samsung.com)
#

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT(trust-anchor)

INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "DEBUG")
ENDIF(NOT CMAKE_BUILD_TYPE)

STRING(REGEX MATCH "([^.]*)" API_VERSION "${LIB_VERSION}")
SET(TANCHOR_INCLUDE ${PROJECT_SOURCE_DIR}/api)
SET(TANCHOR_SRC ${PROJECT_SOURCE_DIR}/src)
SET(TARGET_TANCHOR_LIB ${LIB_NAME})

ADD_DEFINITIONS("-DTANCHOR_USR_DIR=\"${TANCHOR_USR}\"")
ADD_DEFINITIONS("-DTANCHOR_GLOBAL_DIR=\"${TANCHOR_GLOBAL}\"")
ADD_DEFINITIONS("-DTANCHOR_BUNDLE=\"${TANCHOR_BUNDLE}\"")
ADD_DEFINITIONS("-DTZ_SYS_CA_CERTS=\"${TZ_SYS_CA_CERTS}\"")
ADD_DEFINITIONS("-DTZ_SYS_CA_BUNDLE=\"${TZ_SYS_CA_BUNDLE}\"")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
	SET(CXX_STD "c++0x")
else()
	SET(CXX_STD "c++11")
endif()

ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Werror")
ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-Wextra")
ADD_DEFINITIONS("-pedantic")
ADD_DEFINITIONS("-pedantic-errors")

SET(CMAKE_C_FLAGS_PROFILING "-g -O0 -pg")
SET(CMAKE_CXX_FLAGS_PROFILING "-g -O0 -pg -std=${CXX_STD} -fno-rtti")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -ggdb")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -ggdb -std=${CXX_STD} -fno-rtti")
SET(CMAKE_C_FLAGS_RELEASE "-g -O2 -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -O2 -DNDEBUG -std=${CXX_STD} -fno-rtti")
SET(CMAKE_C_FLAGS_CCOV "-g -O0 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV "-g -O0 --coverage -std=${CXX_STD} -fno-rtti")

SET(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS     "-Wl,--as-needed")

IF(NOT DEFINED LIB_INSTALL_DIR)
	SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
ENDIF(NOT DEFINED LIB_INSTALL_DIR)

IF(NOT DEFINED INCLUDE_INSTALL_DIR)
	SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
ENDIF(NOT DEFINED INCLUDE_INSTALL_DIR)

CONFIGURE_FILE(packaging/trust-anchor.manifest.in trust-anchor.manifest @ONLY)

ADD_SUBDIRECTORY(api)
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(src)
