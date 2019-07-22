#
# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
SET(TARGET ${PROJECT_NAME})
SET(PC_FILE "${TARGET}.pc")

SET(LIB_VERSION "${VERSION}")
SET(LIB_SOVERSION "0")

SET(API_SOURCES "wifi.cpp")
SET(API_HEADERS "wifi.h")

SET(DEPENDENCY      klay
					dpm-pil
                    capi-base-common
                    capi-system-info
)

PKG_CHECK_MODULES(API_DEPS REQUIRED ${DEPENDENCY})

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack")

ADD_LIBRARY(${TARGET} SHARED ${API_SOURCES})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fvisibility=default")
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES VERSION   ${LIB_VERSION})

INCLUDE_DIRECTORIES(SYSTEM ${API_DEPS_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${TARGET} ${API_DEPS_LIBRARIES} pthread)

CONFIGURE_FILE(${PC_FILE}.in ${CMAKE_BINARY_DIR}/${PC_FILE} @ONLY)

INSTALL(FILES ${CMAKE_BINARY_DIR}/${PC_FILE} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
INSTALL(TARGETS ${TARGET} DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
INSTALL(FILES ${API_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/dpm)
