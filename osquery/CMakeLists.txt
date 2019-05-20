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
							  gflags
							  librocksdb.a
							  pthread
							  snappy
							  z
							  bz2
							  dl
							  lz4
							  zstd
							  boost_system
							  boost_thread
							  boost_filesystem
							  sqlite3
							  libthrift.a
							  procps
							  systemd)
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
ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(dispatcher)
ADD_SUBDIRECTORY(database)
ADD_SUBDIRECTORY(events)
ADD_SUBDIRECTORY(filesystem)
ADD_SUBDIRECTORY(logger)
ADD_SUBDIRECTORY(registry)
ADD_SUBDIRECTORY(scheduler)
ADD_SUBDIRECTORY(tables)

## Table generation #############################################################
FILE(GLOB TABLE_FILES "tables/specs/x/*.table")
FILE(GLOB TABLE_FILES_LINUX "tables/specs/linux/*.table")
LIST(APPEND TABLE_FILES ${TABLE_FILES_LINUX})

SET(GENERATED_TABLES "")
FOREACH(TABLE_FILE ${TABLE_FILES})
	SET(TABLE_FILE_GEN ${TABLE_FILE})
	STRING(REPLACE "${CMAKE_SOURCE_DIR}/osquery/tables/specs"
				   "${CMAKE_BINARY_DIR}/generated/tables"
				   TABLE_FILE_GEN
				   ${TABLE_FILE_GEN})
	STRING(REPLACE "linux/" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE "x/" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE ".table" ".cpp" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	ADD_CUSTOM_COMMAND(
		OUTPUT ${TABLE_FILE_GEN}
		COMMAND
			python ${CMAKE_SOURCE_DIR}/tools/gentable.py ${TABLE_FILE} ${TABLE_FILE_GEN}
		DEPENDS ${TABLE_FILE}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	LIST(APPEND GENERATED_TABLES ${TABLE_FILE_GEN})
ENDFOREACH()

## Library generation ###########################################################
ADD_LIBRARY(osquery_generated_tables OBJECT ${GENERATED_TABLES})
ADD_LIBRARY(osquery_static STATIC $<TARGET_OBJECTS:osquery_generated_tables>
								  ${${TARGET_OSQUERY_LIB}_SRCS})
SET_TARGET_PROPERTIES(osquery_static PROPERTIES OUTPUT_NAME osquery_static)

# static_lib should include every object file in the archive in the link
ADD_LIBRARY(${TARGET_OSQUERY_LIB} STATIC main/lib.cpp)
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} "-Wl,-whole-archive")
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} osquery_static)
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} "-Wl,-no-whole-archive")
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} ${${TARGET_OSQUERY_LIB}_DEP})
