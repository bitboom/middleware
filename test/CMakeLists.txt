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
PROJECT(klay-test)

SET(TEST_SRC	main.cpp
				rmi.cpp
				xml.cpp
				auth.cpp
				dbus.cpp
				proc.cpp
				misc.cpp
				logger.cpp
				eventfd.cpp
				database.cpp
				filesystem.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_SRC})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES  PREFIX ""
    COMPILE_DEFINITIONS TEST_DATA_DIR="${TEST_DATA_INSTALL_DIR}"
)


PKG_CHECK_MODULES(TEST_DEPS REQUIRED	glib-2.0
										gio-2.0
)

INCLUDE_DIRECTORIES(SYSTEM ${TEST_DEPS_INCLUDE_DIRS} ${KLAY_INCLUDE})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${TEST_DEPS_LIBRARIES} ${LFS_LDFLAGS} ${LFS_LIBS} klay pthread)

INSTALL(TARGETS	${PROJECT_NAME}			DESTINATION bin)
INSTALL(FILES	data/sample-policy.xml	DESTINATION ${TEST_DATA_INSTALL_DIR})
INSTALL(FILES	data/test-proc.sh		DESTINATION ${TEST_DATA_INSTALL_DIR})
