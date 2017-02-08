# Copyright (c) 2017 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Sangwan Kwon (sangwan.kwon@samsung.com)
# @breif
#
SET(TEST_SRCS
	main.cpp
	test-app-custom-trust-anchor.cpp
	test-certificate.cpp
)

SET(TEST_TRANSEC_PATH "${CERT_SVC_TESTS}/transec")
ADD_DEFINITIONS("-DTEST_TRANSEC_PATH=\"${TEST_TRANSEC_PATH}\"")

INCLUDE_DIRECTORIES(
	.
	SYSTEM
	${PROJECT_SOURCE_DIR}/src/transec
)

ADD_EXECUTABLE(${TARGET_TRANSEC_TEST}
	${TEST_SRCS}
	${DPL_TEST_SOURCES}
)

TARGET_LINK_LIBRARIES(${TARGET_TRANSEC_TEST}
	${TARGET_VCORE_LIB}
	${TARGET_TRANSEC_LIB}
	${TEST_DEP_LIBRARIES}
)

INSTALL(TARGETS
			${TARGET_TRANSEC_TEST}
		DESTINATION
			${BIN_DIR}
		PERMISSIONS
			OWNER_READ
			OWNER_WRITE
			OWNER_EXECUTE
			GROUP_READ
			GROUP_EXECUTE
			WORLD_READ
			WORLD_EXECUTE
)

ADD_SUBDIRECTORY(resource)
