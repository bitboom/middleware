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
SET (KLAY_SOURCES		${KLAY_SRC}/error.cpp
						${KLAY_SRC}/process.cpp
						${KLAY_SRC}/eventfd.cpp
						${KLAY_SRC}/mainloop.cpp
						${KLAY_SRC}/testbench.cpp
						${KLAY_SRC}/file-user.cpp
						${KLAY_SRC}/filesystem.cpp
						${KLAY_SRC}/thread-pool.cpp
						${KLAY_SRC}/file-descriptor.cpp
						${KLAY_SRC}/db/column.cpp
						${KLAY_SRC}/db/statement.cpp
						${KLAY_SRC}/db/connection.cpp
						${KLAY_SRC}/xml/node.cpp
						${KLAY_SRC}/xml/parser.cpp
						${KLAY_SRC}/xml/document.cpp
						${KLAY_SRC}/xml/keepblanks.cpp
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
						${KLAY_SRC}/audit/logger.cpp
						${KLAY_SRC}/audit/null-sink.cpp
						${KLAY_SRC}/audit/console-sink.cpp
						${KLAY_SRC}/audit/dlog-sink.cpp
)

SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${KLAY_SOURCES})

PKG_CHECK_MODULES(KLAY_DEPS REQUIRED	gio-2.0
										libxml-2.0
										sqlite3
										dlog
)

INCLUDE_DIRECTORIES(SYSTEM	${KLAY_INCLUDE}
							${KLAY_DEPS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${KLAY_DEPS_LIBRARIES} ${LFS_LDFLAGS} ${LFS_LIBS})

INSTALL(TARGETS	${PROJECT_NAME}	DESTINATION	${LIB_INSTALL_DIR}	COMPONENT DevelopmentLibraries)
INSTALL(DIRECTORY ${KLAY_INCLUDE}/ DESTINATION ${INCLUDE_INSTALL_DIR})
