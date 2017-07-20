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

SET(PIL_TARGET "dpm-pil")
SET(PC_FILE "${PIL_TARGET}.pc")

SET(LIB_VERSION "${VERSION}")
SET(LIB_SOVERSION "0")

SET(PIL				app-bundle.cpp
					launchpad.cpp
					packman.cpp
					policy-event.cpp
					policy-client.cpp
					policy-admin.cpp
					policy-storage.cpp
)

SET(HEADERS			app-bundle.h
					launchpad.h
					packman.h
					policy-event.h
					policy-context.h
					policy-admin.h
					policy-client.h
					policy-model.h
					policy-storage.h
					status.h
					observer.h
					error.h
)

SET(DEPENDENCY		klay
					glib-2.0
					gio-2.0
					sqlite3
					bundle
					aul
					appsvc
					pkgmgr
					pkgmgr-info
					vconf
					syspopup-caller
					deviced
					libtzplatform-config
					security-privilege-manager
					capi-system-info
					capi-system-system-settings
					notification
					cynara-client
					cynara-session
)

PKG_CHECK_MODULES(PIL_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${PIL_DEPS_INCLUDE_DIRS})

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack")

ADD_LIBRARY(${PIL_TARGET} SHARED ${PIL})

SET_TARGET_PROPERTIES(${PIL_TARGET} PROPERTIES COMPILE_FLAGS "-fvisibility=default")
SET_TARGET_PROPERTIES(${PIL_TARGET} PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PIL_TARGET} PROPERTIES VERSION   ${LIB_VERSION})

CONFIGURE_FILE(${PC_FILE}.in ${CMAKE_BINARY_DIR}/${PC_FILE} @ONLY)

TARGET_LINK_LIBRARIES(${PIL_TARGET} ${PIL_DEPS_LIBRARIES} pthread sqlite3)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PC_FILE} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
INSTALL(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/dpm/pil)
INSTALL(TARGETS ${PIL_TARGET} DESTINATION ${LIB_INSTALL_DIR} COMPONENT RuntimeLibraries)
