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
					client-manager.cpp
					policy-builder.cpp
					policy-storage.cpp
					policy-model.cpp
					syspopup.cpp
					preference.cpp
)

SET(SECURITY_MONITOR	dsm/smack.cpp
						dsm/smack_report_issuer.cpp
						dsm/smack_log_parser.cpp
						dsm/smack_kernel_log_collector.cpp
						dsm/smack_kernel_log_parser.cpp
)

SET(POLICY			${SECURITY_MONITOR}
					administration.cpp
					application.cpp
					restriction.cpp
					security.cpp
					storage.cpp
					wifi.cpp
)
SET(POLICY_1		${POLICY}
					bluetooth.cpp
					location.cpp
					password.cpp
					password-manager.cpp
)
SET(POLICY_2		${POLICY_1}
					krate.cpp
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
					jsoncpp
)
IF(NOT DEFINED NO_EXT_PASSWORD)
SET(DEPENDENCY	${DEPENDENCY}	capi-location-manager
					capi-network-bluetooth
					auth-fw-admin
					bluetooth-api
)
IF (NOT DEFINED NO_EXT_KRATE_ZONE)
SET(DEPENDENCY	${DEPENDENCY}	krate
)
ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)
ELSE(NOT DEFINED NO_EXT_PASSWORD)
IF (NOT DEFINED NO_EXT_KRATE_ZONE)
# It is error if !NO_EXT_KRATE_ZONE + NO_EXT_PASSWORD
	error("NO_EXT_KRATE_ZONE should be defined if NO_EXT_PASSWORD is defined.")
ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)
ENDIF(NOT DEFINED NO_EXT_PASSWORD)

PKG_CHECK_MODULES(SERVER_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM ${SERVER_DEPS_INCLUDE_DIRS} ${DPM_POLICY} ${DPM_SERVER} dsm)

ADD_EXECUTABLE(${TARGET}.minimal ${FOUNDATION} ${POLICY})
TARGET_LINK_LIBRARIES(${TARGET}.minimal ${SERVER_DEPS_LIBRARIES} pthread sqlite3)
SET_TARGET_PROPERTIES(${TARGET}.minimal PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET}.minimal PROPERTIES LINK_FLAGS "-pie")
TARGET_COMPILE_DEFINITIONS(${TARGET}.minimal PRIVATE
	DATA_PATH="${DATA_INSTALL_DIR}"
	RUN_PATH="${RUN_INSTALL_DIR}"
	DB_PATH="${DB_INSTALL_DIR}"
)
INSTALL(TARGETS ${TARGET}.minimal DESTINATION bin)


# Add "Password" (no TV). (password, location, bluetooth)
IF(NOT DEFINED NO_EXT_PASSWORD)
  ADD_EXECUTABLE(${TARGET}.ext-pwd ${FOUNDATION} ${POLICY_1})
  TARGET_LINK_LIBRARIES(${TARGET}.ext-pwd ${SERVER_DEPS_LIBRARIES} pthread sqlite3)
  SET_TARGET_PROPERTIES(${TARGET}.ext-pwd PROPERTIES COMPILE_FLAGS "-fPIE")
  SET_TARGET_PROPERTIES(${TARGET}.ext-pwd PROPERTIES LINK_FLAGS "-pie")
  TARGET_COMPILE_DEFINITIONS(${TARGET}.ext-pwd PRIVATE
  	DATA_PATH="${DATA_INSTALL_DIR}"
  	RUN_PATH="${RUN_INSTALL_DIR}"
  	DB_PATH="${DB_INSTALL_DIR}"
  )
  INSTALL(TARGETS ${TARGET}.ext-pwd DESTINATION bin)

# Add "Krate" (no TV, WEARABLE). (krate)
IF (NOT DEFINED NO_EXT_KRATE_ZONE)
  ADD_EXECUTABLE(${TARGET} ${FOUNDATION} ${POLICY_2})
  TARGET_LINK_LIBRARIES(${TARGET} ${SERVER_DEPS_LIBRARIES} pthread sqlite3)
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
  SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS "-pie")
  TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
  	DATA_PATH="${DATA_INSTALL_DIR}"
  	RUN_PATH="${RUN_INSTALL_DIR}"
  	DB_PATH="${DB_INSTALL_DIR}"
  )
  INSTALL(TARGETS ${TARGET} DESTINATION bin)

ENDIF(NOT DEFINED NO_EXT_KRATE_ZONE)
ENDIF(NOT DEFINED NO_EXT_PASSWORD)

CONFIGURE_FILE(systemd/device-policy-manager.service.in systemd/device-policy-manager.service)

INSTALL(FILES systemd/device-policy-manager.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})
