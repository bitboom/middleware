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

SET(TEST_SRCS main.cpp
			  test-util.cpp
			  test-client.cpp
			  test-admin.cpp
			  test-sw-backend.cpp

			  ${SERVER_PATH}/service/plugin-loader.cpp
			  ${SERVER_PATH}/service/plugin-manager.cpp)

PKG_CHECK_MODULES(${TARGET_TEST}_DEP REQUIRED klay)

INCLUDE_DIRECTORIES(SYSTEM ${${TARGET_TEST}_DEP_INCLUDE_DIRS}
						   ${INCLUDE_PATH}
						   ${SERVER_PATH}/service/include
						   ${DPL_PATH}/core/include
						   ${DPL_PATH}/log/include
						   ${COMMON_PATH}/include
						   ${PLUGIN_PATH})

ADD_EXECUTABLE(${TARGET_TEST} ${TEST_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_TEST} ${${TARGET_TEST}_DEP_LIBRARIES}
									 ${TARGET_CLIENT}
									 ${TARGET_CLIENT_ADMIN}
									 ${TARGET_SW_BACKEND}
									 -ldl)

SET_TARGET_PROPERTIES(${TARGET_TEST} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET_TEST} PROPERTIES LINK_FLAGS "-pie")

INSTALL(TARGETS ${TARGET_TEST}
		DESTINATION bin
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
