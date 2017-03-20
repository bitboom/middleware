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
# @file        CMakeLists.txt
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
# @breif       Make trust anchor test program
#

INCLUDE_DIRECTORIES(SYSTEM .
						   ${TANCHOR_INCLUDE}
						   ${TANCHOR_SRC}
						   ${${TARGET_TANCHOR_LIB}_DEP_INCLUDE_DIRS})

FUNCTION(BUILD_TEST TEST_NAME TEST_SRCS)
	ADD_EXECUTABLE(${TEST_NAME} ${TEST_SRCS})
	TARGET_LINK_LIBRARIES(${TEST_NAME} ${TARGET_TANCHOR_LIB})
	SET_SOURCE_FILES_PROPERTIES(${TEST_SRCS}
		PROPERTIES
			COMPILE_FLAGS "-D_GNU_SOURCE -fPIE")
	INSTALL(TARGETS ${TEST_NAME}
			DESTINATION ${BIN_INSTALL_DIR}
			PERMISSIONS OWNER_READ
						OWNER_WRITE
						OWNER_EXECUTE
						GROUP_READ
						GROUP_EXECUTE
						WORLD_READ
						WORLD_EXECUTE)
ENDFUNCTION(BUILD_TEST)

ADD_DEFINITIONS("-DTZ_SYS_RO_CA_CERTS=\"${TZ_SYS_RO_CA_CERTS}\"")
ADD_DEFINITIONS("-DTZ_SYS_RO_CA_BUNDLE=\"${TZ_SYS_RO_CA_BUNDLE}\"")

SET(INSTALLER_SRCS main.cpp
				   test-capi-installer.cpp
				   test-installer.cpp)
SET(LAUNCHER_SRCS main.cpp
				  test-util.cpp
				  test-launcher.cpp)
SET(CAPI_LAUNCHER_SRCS main.cpp
					   test-util.cpp
					   test-capi-launcher.cpp)
SET(INTERNAL_SRCS main.cpp
				  test-certificate.cpp)

BUILD_TEST("${TARGET_TANCHOR_TEST}-installer" "${INSTALLER_SRCS}")
BUILD_TEST("${TARGET_TANCHOR_TEST}-launcher" "${LAUNCHER_SRCS}")
BUILD_TEST("${TARGET_TANCHOR_TEST}-capi-launcher" "${CAPI_LAUNCHER_SRCS}")
BUILD_TEST("${TARGET_TANCHOR_TEST}-internal" "${INTERNAL_SRCS}")

ADD_SUBDIRECTORY(res)
ADD_SUBDIRECTORY(script)
