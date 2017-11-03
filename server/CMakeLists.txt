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

SET(TARGET ${PROJECT_NAME})
SET(SYSPOPUPEXE device-policy-syspopup)

SET(DAEMON			main.cpp
					server.cpp
					sql-backend.cpp
					plugin.cpp
)

SET(SYSPOPUP		syspopup.cpp
					syspopup-service.cpp
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
					cynara-client
					cynara-session
)

PKG_CHECK_MODULES(SERVER_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${SERVER_DEPS_INCLUDE_DIRS} ${DPM_POLICY} ${DPM_SERVER} ${DPM_LIBS})

ADD_EXECUTABLE(${TARGET} ${DAEMON} ${POLICY})
TARGET_LINK_LIBRARIES(${TARGET} ${SERVER_DEPS_LIBRARIES} dl pthread sqlite3 dpm-pil)
ADD_EXECUTABLE(${SYSPOPUPEXE} ${SYSPOPUP})
TARGET_LINK_LIBRARIES(${SYSPOPUPEXE} ${SERVER_DEPS_LIBRARIES} dpm-pil)

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS "-pie")
SET_TARGET_PROPERTIES(${SYSPOPUPEXE} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${SYSPOPUPEXE} PROPERTIES LINK_FLAGS "-pie")

TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
	DATA_PATH="${DATA_INSTALL_DIR}"
	RUN_PATH="${RUN_INSTALL_DIR}"
	DB_PATH="${DB_INSTALL_DIR}"
	PLUGIN_INSTALL_DIR="${PLUGIN_INSTALL_DIR}"
)

INSTALL(TARGETS ${TARGET} DESTINATION bin)
INSTALL(TARGETS ${SYSPOPUPEXE} DESTINATION bin)

CONFIGURE_FILE(systemd/device-policy-manager.service.in systemd/device-policy-manager.service)
CONFIGURE_FILE(systemd/${SYSPOPUPEXE}.service.in systemd/${SYSPOPUPEXE}.service)

INSTALL(FILES systemd/device-policy-manager.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})
INSTALL(FILES systemd/${SYSPOPUPEXE}.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})

INSTALL(FILES dbus/org.tizen.dpm.syspopup.conf DESTINATION ${DUBS_CONFIGURE_DIR})
INSTALL(FILES dbus/org.tizen.dpm.syspopup.service DESTINATION ${DBUS_SERVICE_DIR})
