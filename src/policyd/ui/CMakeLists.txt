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

SET(TARGET_SYSPOPUP device-policy-syspopup)

# TODO: minimize dependencies
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

SET(SYSPOPUP syspopup.cpp
			 syspopup-service.cpp)


PKG_CHECK_MODULES(SERVER_DEPS REQUIRED ${SERVER_DEP})

INCLUDE_DIRECTORIES(${SERVER_DEPS_INCLUDE_DIRS})

ADD_EXECUTABLE(${TARGET_SYSPOPUP} ${SYSPOPUP})
TARGET_LINK_LIBRARIES(${TARGET_SYSPOPUP} ${SERVER_DEPS_LIBRARIES} policyd)

SET_TARGET_PROPERTIES(${TARGET_SYSPOPUP} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET_SYSPOPUP} PROPERTIES LINK_FLAGS "-pie")

INSTALL(TARGETS ${TARGET_SYSPOPUP} DESTINATION bin)
