# Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
SET(TARGET "vist-bluetooth-policy")
SET(TARGET_TEST ${TARGET}-test)

SET(DEPENDENCY bluetooth-api
			   capi-network-bluetooth)

PKG_CHECK_MODULES(PLUGIN_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${PLUGIN_DEPS_INCLUDE_DIRS})

## LIB ###########################################
ADD_LIBRARY(${TARGET} SHARED policy.cpp table.cpp)
TARGET_LINK_LIBRARIES(${TARGET} ${PLUGIN_DEPS_LIBRARIES}
								${TARGET_VIST_COMMON_LIB}
								${TARGET_VIST_POLICY_LIB})

INSTALL(TARGETS ${TARGET} DESTINATION ${TABLE_INSTALL_DIR})

## TEST #############################################
ADD_EXECUTABLE(${TARGET_TEST} ../../../main/tests.cpp
							  policy.cpp
							  table.cpp
							  tests/bluetooth.cpp)
TARGET_LINK_LIBRARIES(${TARGET_TEST} ${PLUGIN_DEPS_LIBRARIES}
									 ${TARGET_VIST_CLIENT_LIB}
									 ${TARGET_VIST_LIB}
									 gtest
									 pthread)
INSTALL(TARGETS ${TARGET_TEST}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
