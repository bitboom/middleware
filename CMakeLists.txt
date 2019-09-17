#  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(OSQUERY)

INCLUDE(GNUInstallDirs)
INCLUDE(CMake/Macro.cmake)

IF(DEFINED GBS_BUILD)
	INCLUDE(FindPkgConfig)
ENDIF(DEFINED GBS_BUILD)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG   "-g -std=c++1y -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++1y -O2 -DNDEBUG")

SET(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS     "-Wl,--as-needed")

ADD_DEFINITIONS("-fPIC")
#ADD_DEFINITIONS("-Werror")
#ADD_DEFINITIONS("-Wall")
#ADD_DEFINITIONS("-Wextra")
#ADD_DEFINITIONS("-pedantic")
#ADD_DEFINITIONS("-pedantic-errors")

# TODO(sangwan.kwon): Get version from packing spec.
SET(OSQUERY_VERSION "4.0.0")

# Set various platform/platform-version/build version/etc defines.
ADD_DEFINITIONS(-DLINUX="1"
				-DPOSIX="1"
				-DOSQUERY_LINUX="1"
				-DOSQUERY_POSIX="1"
				-DOSQUERY_BUILD_PLATFORM="linux"
				-DOSQUERY_BUILD_DISTRO="linux")

ADD_DEFINITIONS(-DOSQUERY_VERSION=${OSQUERY_VERSION}
				-DOSQUERY_BUILD_VERSION=${OSQUERY_VERSION}
				-DOSQUERY_BUILD_SDK_VERSION=${OSQUERY_VERSION})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/api")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/tools/sqlite3")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/osquery/include")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/apix/tsqb")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/osquery/extensions/thrift/gen")
INCLUDE_DIRECTORIES("/usr/local/include")

ENABLE_TESTING()

# Make sure the generated paths exist
EXECUTE_PROCESS(COMMAND mkdir -p "${CMAKE_BINARY_DIR}/generated")

CONFIGURE_FILE(packaging/device-policy-manager.manifest.in
			   device-policy-manager.manifest @ONLY)

ADD_SUBDIRECTORY(specs)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tools/sqlite3)
IF(DEFINED GBS_BUILD)
	ADD_SUBDIRECTORY(plugins)
ENDIF()
