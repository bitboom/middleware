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

SET(TARGET_POLICYD_TEST policyd-test)

SET(${TARGET_POLICYD_LIB}_SRCS "")
SET(${TARGET_POLICYD_LIB}_TESTS "")

SET(DEPENDENCY klay)

PKG_CHECK_MODULES(POLICYD_DEPS REQUIRED ${DEPENDENCY})

INCLUDE_DIRECTORIES(SYSTEM . ${POLICYD_DEPS_INCLUDE_DIRS})

ADD_DEFINITIONS(-DDATA_PATH="${DATA_INSTALL_DIR}"
				-DRUN_PATH="${RUN_INSTALL_DIR}"
				-DDB_PATH="${DB_INSTALL_DIR}/.dpm.db"
				-DPLUGIN_INSTALL_DIR="${PLUGIN_INSTALL_DIR}"
				-DEVENT_CONFIGURE_DIR="${EVENT_CONFIGURE_DIR}")
ADD_DEFINITIONS(-DUG_WAYLAND)

ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(sdk)

#ADD_SUBDIRECTORY(client)
#ADD_SUBDIRECTORY(conf)
#ADD_SUBDIRECTORY(pil)
#ADD_SUBDIRECTORY(server)
#ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(ui)

ADD_LIBRARY(${TARGET_POLICYD_LIB} STATIC ${${TARGET_POLICYD_LIB}_SRCS})

TARGET_LINK_LIBRARIES(${TARGET_POLICYD_LIB} ${POLICYD_DEPS_LIBRARIES} pthread dl)

SET_TARGET_PROPERTIES(${TARGET_POLICYD_LIB} PROPERTIES COMPILE_FLAGS "-fPIE")
SET_TARGET_PROPERTIES(${TARGET_POLICYD_LIB} PROPERTIES LINK_FLAGS "-pie")


ADD_EXECUTABLE(${TARGET_POLICYD_TEST} ../apix/main/tests.cpp
									  ${${TARGET_POLICYD_LIB}_TESTS})

TARGET_LINK_LIBRARIES(${TARGET_POLICYD_TEST} ${TARGET_POLICYD_LIB}
											 gtest
											 pthread)
ADD_TEST(${TARGET_POLICYD_TEST} ${TARGET_POLICYD_TEST})
INSTALL(TARGETS ${TARGET_POLICYD_TEST}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
