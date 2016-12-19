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

SET(FOUNDATION		main.cpp
					server.cpp
					app-bundle.cpp
					launchpad.cpp
					packman.cpp
					policy-builder.cpp
					policy-manager.cpp
					syspopup.cpp
					preference.cpp
)

SET(POLICY			administration.cpp
					application.cpp
					restriction.cpp
					security.cpp
					storage.cpp
					wifi.cpp
)

SET(DEPENDENCY		klay
					glib-2.0
					gio-2.0
					libxml-2.0
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
					bluetooth-api
					capi-network-wifi
					capi-system-info
					capi-system-system-settings
					capi-network-connection
					capi-network-bluetooth
					notification
					cynara-client
					cynara-session
)

INCLUDE("${TIZEN_PROFILE_NAME}.cmake")

ADD_EXECUTABLE(${TARGET} ${FOUNDATION} ${POLICY})

PKG_CHECK_MODULES(SERVER_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${SERVER_DEPS_INCLUDE_DIRS} ${DPM_POLICY} ${DPM_SERVER})

TARGET_LINK_LIBRARIES(${TARGET} ${SERVER_DEPS_LIBRARIES} pthread sqlite3)

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS "-pie")

TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
	DATA_PATH="${DATA_INSTALL_DIR}"
	RUN_PATH="${RUN_INSTALL_DIR}"
	DB_PATH="${DB_INSTALL_DIR}"
)

CONFIGURE_FILE(systemd/device-policy-manager.service.in systemd/device-policy-manager.service)

INSTALL(TARGETS ${TARGET} DESTINATION bin)
INSTALL(FILES systemd/device-policy-manager.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})
