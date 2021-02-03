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
SET(LIB_VERSION "${VERSION}")
SET(LIB_SOVERSION "0")

SET (KLAY_SOURCES		${KLAY_SRC}/error.cpp
						${KLAY_SRC}/exception.cpp
						${KLAY_SRC}/process.cpp
						${KLAY_SRC}/eventfd.cpp
						${KLAY_SRC}/mainloop.cpp
						${KLAY_SRC}/testbench.cpp
						${KLAY_SRC}/testbench/test-case.cpp
						${KLAY_SRC}/testbench/test-suite.cpp
						${KLAY_SRC}/testbench/test-driver.cpp
						${KLAY_SRC}/testbench/test-reporter.cpp
						${KLAY_SRC}/file-user.cpp
						${KLAY_SRC}/filesystem.cpp
						${KLAY_SRC}/thread-pool.cpp
						${KLAY_SRC}/file-descriptor.cpp
						${KLAY_SRC}/db/column.cpp
						${KLAY_SRC}/db/statement.cpp
						${KLAY_SRC}/db/connection.cpp
						${KLAY_SRC}/rmi/socket.cpp
						${KLAY_SRC}/rmi/client.cpp
						${KLAY_SRC}/rmi/service.cpp
						${KLAY_SRC}/rmi/message.cpp
						${KLAY_SRC}/rmi/connection.cpp
						${KLAY_SRC}/rmi/notification.cpp
						${KLAY_SRC}/rmi/message-composer.cpp
						${KLAY_SRC}/auth/user.cpp
						${KLAY_SRC}/auth/group.cpp
						${KLAY_SRC}/dbus/error.cpp
						${KLAY_SRC}/dbus/variant.cpp
						${KLAY_SRC}/dbus/connection.cpp
						${KLAY_SRC}/dbus/introspection.cpp
						${KLAY_SRC}/dbus/signal.cpp
						${KLAY_SRC}/audit/logger.cpp
						${KLAY_SRC}/audit/logger-core.cpp
						${KLAY_SRC}/audit/null-sink.cpp
						${KLAY_SRC}/audit/console-sink.cpp
						${KLAY_SRC}/audit/dlog-sink.cpp
						${KLAY_SRC}/audit/audit-trail.cpp
						${KLAY_SRC}/netlink/netlink.cpp
)

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${KLAY_SOURCES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION   ${LIB_VERSION})

ADD_LIBRARY(${PROJECT_NAME}-static STATIC ${KLAY_SOURCES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION ${LIB_SOVERSION})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION   ${LIB_VERSION})

PKG_CHECK_MODULES(KLAY_DEPS REQUIRED	gio-2.0
										sqlite3
										dlog
										libsystemd
)

INCLUDE_DIRECTORIES(SYSTEM	${KLAY_INCLUDE}
							${KLAY_DEPS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${KLAY_DEPS_LIBRARIES} ${LFS_LDFLAGS} ${LFS_LIBS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static ${KLAY_DEPS_LIBRARIES} ${LFS_LDFLAGS} ${LFS_LIBS})

INSTALL(TARGETS	${PROJECT_NAME}			DESTINATION	${LIB_INSTALL_DIR}	COMPONENT DevelopmentLibraries)
INSTALL(TARGETS	${PROJECT_NAME}-static	DESTINATION	${LIB_INSTALL_DIR}	COMPONENT DevelopmentLibraries)
INSTALL(DIRECTORY ${KLAY_INCLUDE}/		DESTINATION ${INCLUDE_INSTALL_DIR})
