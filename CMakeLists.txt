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

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS_DEBUG   "-g -std=c++11 -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++11 -O2 -DNDEBUG")

SET(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS     "-Wl,--as-needed")

ADD_DEFINITIONS("-fPIC")
#ADD_DEFINITIONS("-Werror")
#ADD_DEFINITIONS("-Wall")
#ADD_DEFINITIONS("-Wextra")
#ADD_DEFINITIONS("-pedantic")
#ADD_DEFINITIONS("-pedantic-errors")

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("/usr/local/include")

ENABLE_TESTING()

ADD_SUBDIRECTORY(osquery)
