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

IF(NOT DEFINED INCLUDE_INSTALL_DIR)
	SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
ENDIF(NOT DEFINED INCLUDE_INSTALL_DIR)

SET(TANCHOR_INCLUDE ${PROJECT_SOURCE_DIR}/api/tanchor)

CONFIGURE_FILE(packaging/trust-anchor.manifest trust-anchor.manifest)

ADD_SUBDIRECTORY(api)
