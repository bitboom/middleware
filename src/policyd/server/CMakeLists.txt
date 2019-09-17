# Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

SET(TARGET device-policy-manager)
SET(TARGET_SERVER_LIB dpm-server)

SET(SERVER_SRC server.cpp
			   sql-backend.cpp
			   plugin.cpp)

SET(SERVER_DEP klay
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
			   cynara-session)

PKG_CHECK_MODULES(SERVER_DEPS REQUIRED ${SERVER_DEP})

INCLUDE_DIRECTORIES(${SERVER_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_SERVER_LIB} STATIC ${SERVER_SRC})
TARGET_LINK_LIBRARIES(${TARGET_SERVER_LIB} ${SERVER_DEPS_LIBRARIES} -ldl)

TARGET_COMPILE_DEFINITIONS(${TARGET_SERVER_LIB} PRIVATE
	DATA_PATH="${DATA_INSTALL_DIR}"
	RUN_PATH="${RUN_INSTALL_DIR}"
	DB_PATH="${DB_INSTALL_DIR}"
	PLUGIN_INSTALL_DIR="${PLUGIN_INSTALL_DIR}")

ADD_EXECUTABLE(${TARGET} main.cpp)
TARGET_LINK_LIBRARIES(${TARGET} dl pthread sqlite3 dpm-pil ${TARGET_SERVER_LIB})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES LINK_FLAGS "-pie")

INSTALL(TARGETS ${TARGET} DESTINATION bin)
