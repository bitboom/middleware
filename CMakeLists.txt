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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(vist)

INCLUDE(GNUInstallDirs)
INCLUDE(CMake/Macro.cmake)

IF(DEFINED GBS_BUILD)
	INCLUDE(FindPkgConfig)
ELSE(DEFINED GBS_BUILD)
	INCLUDE(CMake/Definition.cmake)
ENDIF(DEFINED GBS_BUILD)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "DEBUG")
ENDIF(NOT CMAKE_BUILD_TYPE)

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

SET(CMAKE_CXX_FLAGS_DEBUG   "-g -std=c++1z -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++1z -O2 -DNDEBUG")

SET(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS     "-Wl,--as-needed")

SET(VIST_SRCS        ${CMAKE_SOURCE_DIR}/src/vist)
SET(VIST_TABLES_SRCS ${CMAKE_SOURCE_DIR}/tables)

SET(TARGET_OSQUERY_LIB osquery)
SET(TARGET_VIST_CLIENT_LIB vist-client)
SET(TARGET_VIST_COMMON_LIB vist-common)
SET(TARGET_VIST_LIB vist)
SET(TARGET_VIST_POLICY_LIB vist-policy)

ADD_DEFINITIONS("-fPIC")

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")
# Suppresse SYSTEM header's warnings:
INCLUDE_DIRECTORIES(SYSTEM "${CMAKE_SOURCE_DIR}/src/osquery/include")
INCLUDE_DIRECTORIES("/usr/local/include")

ENABLE_TESTING()

ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tables)

IF(DEFINED GBS_BUILD)
	ADD_SUBDIRECTORY(systemd)
ENDIF(DEFINED GBS_BUILD)
