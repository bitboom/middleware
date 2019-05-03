#  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License

SET(${TARGET_OSQUERY_LIB} osquery)

FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTEST_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(SYSTEM ${TARGET_OSQUERY_LIB}_INCLUDE_DIRS)

MACRO(ADD_OSQUERY_TEST TEST_NAME SOURCE)
	ADD_EXECUTABLE(${TEST_NAME} ${SOURCE})
	TARGET_LINK_LIBRARIES(${TEST_NAME} gtest pthread)
	ADD_TEST(${TEST_NAME} ${TEST_NAME})
	INSTALL(TARGETS ${TEST_NAME}
			DESTINATION ${CMAKE_INSTALL_BINDIR}
			PERMISSIONS OWNER_READ
						OWNER_WRITE
						OWNER_EXECUTE
						GROUP_READ
						GROUP_EXECUTE
						WORLD_READ
						WORLD_EXECUTE)
ENDMACRO(ADD_OSQUERY_TEST)

ADD_SUBDIRECTORY(core)
