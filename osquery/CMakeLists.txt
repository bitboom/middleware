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

SET(TARGET_OSQUERY_LIB osquery)
SET(${TARGET_OSQUERY_LIB}_DEP glog
							  librocksdb.a
							  pthread
							  snappy
							  z
							  bz2
							  lz4
							  zstd
							  boost_system
							  boost_filesystem)
SET(${TARGET_OSQUERY_LIB}_SRCS "")

MACRO(ADD_OSQUERY_LIBRARY TARGET)
	ADD_LIBRARY(${TARGET} OBJECT ${ARGN})
	LIST(APPEND ${TARGET_OSQUERY_LIB}_SRCS $<TARGET_OBJECTS:${TARGET}>)
	SET(${TARGET_OSQUERY_LIB}_SRCS ${${TARGET_OSQUERY_LIB}_SRCS} PARENT_SCOPE)
ENDMACRO(ADD_OSQUERY_LIBRARY TARGET)

MACRO(ADD_OSQUERY_TEST TEST_NAME SOURCE)
	ADD_EXECUTABLE(${TEST_NAME} ${SOURCE})
	TARGET_LINK_LIBRARIES(${TEST_NAME} ${TARGET_OSQUERY_LIB})
	TARGET_LINK_LIBRARIES(${TEST_NAME} gtest)
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
ADD_SUBDIRECTORY(database)

ADD_LIBRARY(${TARGET_OSQUERY_LIB} STATIC main/lib.cpp ${${TARGET_OSQUERY_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} ${${TARGET_OSQUERY_LIB}_DEP})
