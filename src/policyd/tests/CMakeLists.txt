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

SET(DPM_TEST_TARGET "dpm-test")
SET(DPM_TEST_SRCS main.cpp
				  test-pil.cpp
				  test-server.cpp
				  test-query.cpp)

PKG_CHECK_MODULES(${DPM_TEST_TARGET}_DEP REQUIRED klay glib-2.0 gio-2.0 vconf aul)

INCLUDE_DIRECTORIES(SYSTEM ${${DPM_TEST_TARGET}_DEP_INCLUDE_DIRS}
						   ${CMAKE_SOURCE_DIR}/src)

ADD_EXECUTABLE(${DPM_TEST_TARGET} ${DPM_TEST_SRCS})

TARGET_LINK_LIBRARIES(${DPM_TEST_TARGET} ${${DPM_TEST_TARGET}_DEP_LIBRARIES}
										 dpm-server
										 dpm-pil)

SET_TARGET_PROPERTIES(${DPM_TEST_TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${DPM_TEST_TARGET} PROPERTIES LINK_FLAGS "-pie")

INSTALL(TARGETS ${DPM_TEST_TARGET}
		DESTINATION bin
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
