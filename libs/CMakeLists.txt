#
# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

PROJECT(dpm)
SET(LIB_VERSION "${VERSION}")
SET(LIB_SOVERSION "0")

SET(PC_FILE "${PROJECT_NAME}.pc")

SET(FOUNDATION          policy-client.cpp
						dpm/client-handle.cpp
)

SET(POLICY              administration.cpp
						application.cpp
						bluetooth.cpp
						location.cpp
						restriction.cpp
						security.cpp
						storage.cpp
						wifi.cpp
						dpm/administration.cpp
						dpm/application.cpp
						dpm/bluetooth.cpp
						dpm/restriction.cpp
						dpm/security.cpp
						dpm/wifi.cpp
)
# wearable, ivi, common, mobile
SET(POLICY_EXT_PASSWORD	password.cpp
						dpm/password.cpp
)
# ivi, common, mobile
SET(POLICY_EXT_KRATE_ZONE	krate.cpp
						dpm/zone.cpp
)

SET(CAPI                dpm/device-policy-manager.h
						dpm/administration.h
						dpm/application.h
						dpm/bluetooth.h
						dpm/restriction.h
						dpm/security.h
						dpm/wifi.h
)

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${FOUNDATION} ${POLICY})

IF(DEFINED NO_EXT_PASSWORD)
MESSAGE("Password disabled")
ELSE(DEFINED NO_EXT_PASSWORD)
ADD_LIBRARY(${PROJECT_NAME}-ext-password SHARED ${FOUNDATION} ${POLICY_EXT_PASSWORD})
SET(CAPI		${CAPI}
						dpm/password.h
						dpm/password_internal.h
)
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-password PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-password PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-password PROPERTIES VERSION   ${LIB_VERSION})
ENDIF(DEFINED NO_EXT_PASSWORD)

IF(DEFINED NO_EXT_KRATE_ZONE)
MESSAGE("Krate, Zone disabled")
ELSE(DEFINED NO_EXT_KRATE_ZONE)
ADD_LIBRARY(${PROJECT_NAME}-ext-krate_zone SHARED ${FOUNDATION} ${POLICY_EXT_KRATE_ZONE})
SET(CAPI		${CAPI}
						dpm/zone.h
)
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-krate_zone PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-krate_zone PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PROJECT_NAME}-ext-krate_zone PROPERTIES VERSION   ${LIB_VERSION})
ENDIF(DEFINED NO_EXT_KRATE_ZONE)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION   ${LIB_VERSION})

PKG_CHECK_MODULES(LIBS_DEPS	REQUIRED
							klay
							glib-2.0
							capi-base-common
							capi-system-info
)

INCLUDE_DIRECTORIES(SYSTEM ${LIBS_DEPS_INCLUDE_DIRS} ${DPM_POLICY} ${DPM_LIBS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS_DEPS_LIBRARIES} pthread)

CONFIGURE_FILE(${PC_FILE}.in ${CMAKE_BINARY_DIR}/${PC_FILE} @ONLY)

INSTALL(FILES ${CMAKE_BINARY_DIR}/${PC_FILE} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
IF(NOT DEFINED NO_EXT_PASSWORD)
INSTALL(TARGETS ${PROJECT_NAME}-ext-password DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
ENDIF(NOT DEFINED NO_EXT_PASSWORD)
IF(NOT DEFINED NO_EXT_KRATE_ZONE)
INSTALL(TARGETS ${PROJECT_NAME}-ext-krate_zone DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)
INSTALL(FILES ${CAPI} DESTINATION ${INCLUDE_INSTALL_DIR}/dpm)

#Tizen Device Security Monitor Client Library
ADD_SUBDIRECTORY(dsm)