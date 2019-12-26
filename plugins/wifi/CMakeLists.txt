# Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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
SET(TARGET "vist-plugin-wifi")
SET(TARGET_TEST ${TARGET}-test)

SET(PLUGIN_SOURCES "wifi.cpp")

SET(DEPENDENCY klay
			   capi-network-wifi-manager
			   capi-network-connection)

PKG_CHECK_MODULES(PLUGIN_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${PLUGIN_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET} SHARED ${PLUGIN_SOURCES})
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fvisibility=default")
TARGET_LINK_LIBRARIES(${TARGET} ${PLUGIN_DEPS_LIBRARIES}
								vist-common)
INSTALL(FILES libvist-plugin-wifi.so
		RENAME wifi
		DESTINATION ${PLUGIN_INSTALL_DIR})

ADD_EXECUTABLE(${TARGET_TEST} ../tests.cpp
							  wifi.cpp
							  wifi-test.cpp)
TARGET_LINK_LIBRARIES(${TARGET_TEST} ${PLUGIN_DEPS_LIBRARIES}
									 vist-common
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
