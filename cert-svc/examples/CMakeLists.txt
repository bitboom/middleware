#
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

ADD_DEFINITIONS("-DCERT_SVC_EXAMPLES=\"${CERT_SVC_EXAMPLES}\"")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

FUNCTION(BUILD_EXAMPLE EXAMPLE_NAME SOURCE_FILE)
	ADD_EXECUTABLE(${EXAMPLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})
	TARGET_LINK_LIBRARIES(${EXAMPLE_NAME} ${TARGET_VCORE_LIB})
	INSTALL(TARGETS ${EXAMPLE_NAME}
			DESTINATION ${BIN_DIR}
			PERMISSIONS OWNER_READ
						OWNER_WRITE
						OWNER_EXECUTE
						GROUP_READ
						GROUP_EXECUTE
						WORLD_READ
						WORLD_EXECUTE)
	INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
			DESTINATION ${CERT_SVC_EXAMPLES})
ENDFUNCTION(BUILD_EXAMPLE)

BUILD_EXAMPLE("cert-svc-example-vcore-until-3.0" signature-validator-until-3.0.cpp)
BUILD_EXAMPLE("cert-svc-example-vcore-since-4.0" signature-validator-since-4.0.cpp)

INSTALL(DIRECTORY resource
		DESTINATION ${CERT_SVC_EXAMPLES})
