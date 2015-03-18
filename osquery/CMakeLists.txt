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
SET(TARGET_OSQUERY_LIB_ADDITIONAL osquery_additional)
SET(TARGET_OSQUERY_SHELL osqueryi)
SET(TARGET_OSQUERY_DAEMON osqueryd)

SET(${TARGET_OSQUERY_LIB}_DEP glog
							  gflags
							  pthread
							  libthrift.a
							# rocksdb deps
							  librocksdb.a
							  snappy
							  z
							  bz2
							  dl
							  lz4
							  zstd
							  boost_regex
							  boost_system
							  boost_thread
							  boost_filesystem
							  crypto # openssl
							# shell deps
							  readline
							# build-in tables deps
							  systemd
							  udev)
SET(${TARGET_OSQUERY_LIB}_SRCS "")
SET(${TARGET_OSQUERY_LIB_ADDITIONAL}_SRCS "")

SET(OSQUERY_CODEGEN_PATH "${CMAKE_SOURCE_DIR}/tools/codegen")
SET(OSQUERY_TABLES_PATH "${CMAKE_SOURCE_DIR}/osquery/tables")
SET(OSQUERY_GENERATED_PATH "${CMAKE_BINARY_DIR}/generated")

ADD_DEFINITIONS("-DOSQUERY_BUILD_VERSION=${OSQUERY_BUILD_VERSION}")

MACRO(ADD_OSQUERY_LIBRARY IS_CORE TARGET)
	ADD_LIBRARY(${TARGET} OBJECT ${ARGN})

	IF(${IS_CORE})
		LIST(APPEND ${TARGET_OSQUERY_LIB}_SRCS $<TARGET_OBJECTS:${TARGET}>)
		SET(${TARGET_OSQUERY_LIB}_SRCS ${${TARGET_OSQUERY_LIB}_SRCS} PARENT_SCOPE)
	ELSE()
		LIST(APPEND ${TARGET_OSQUERY_LIB_ADDITIONAL}_SRCS $<TARGET_OBJECTS:${TARGET}>)
		SET(${TARGET_OSQUERY_LIB_ADDITIONAL}_SRCS ${${TARGET_OSQUERY_LIB_ADDITIONAL}_SRCS} PARENT_SCOPE)
	ENDIF()
ENDMACRO(ADD_OSQUERY_LIBRARY TARGET)

MACRO(ADD_OSQUERY_TEST IS_CORE TEST_NAME SOURCE)
	ADD_EXECUTABLE(${TEST_NAME} ${SOURCE})

	TARGET_LINK_LIBRARIES(${TEST_NAME} ${TARGET_OSQUERY_LIB})
	TARGET_LINK_LIBRARIES(${TEST_NAME} gtest)
#	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-DGTEST_HAS_TR1_TUPLE=0")
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

MACRO(TARGET_OSQUERY_LINK_WHOLE TARGET LIBRARY)
	TARGET_LINK_LIBRARIES(${TARGET} "-Wl,-whole-archive")
	TARGET_LINK_LIBRARIES(${TARGET} ${LIBRARY})
	TARGET_LINK_LIBRARIES(${TARGET} "-Wl,-no-whole-archive")
ENDMACRO(TARGET_OSQUERY_LINK_WHOLE)

MACRO(ADD_OSQUERY_MODULE TARGET)
	ADD_LIBRARY(${TARGET} SHARED ${ARGN})
	TARGET_LINK_LIBRARIES(${TARGET} dl)
	ADD_DEPENDENCIES(${TARGET} ${TARGET_OSQUERY_LIB} glog)
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fPIC")
	SET_TARGET_PROPERTIES(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET})
ENDMACRO(ADD_OSQUERY_MODULE)

ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(dispatcher)
ADD_SUBDIRECTORY(distributed)
ADD_SUBDIRECTORY(devtools)
ADD_SUBDIRECTORY(database)
ADD_SUBDIRECTORY(events)
ADD_SUBDIRECTORY(extensions)
ADD_SUBDIRECTORY(filesystem)
ADD_SUBDIRECTORY(logger)
ADD_SUBDIRECTORY(registry)
ADD_SUBDIRECTORY(sql)
ADD_SUBDIRECTORY(tables)

## Table generation #############################################################
FILE(GLOB TABLE_FILES "tables/specs/x/*.table")
FILE(GLOB TABLE_FILES_LINUX "tables/specs/linux/*.table")
LIST(APPEND TABLE_FILES ${TABLE_FILES_LINUX})

SET(GENERATED_TABLES "")

FILE(GLOB TABLE_FILES_TEMPLATES "tables/templates/*.in")
SET(GENERATION_DEPENDENCIES "${OSQUERY_CODEGEN_PATH}/gentable.py"
							"${OSQUERY_CODEGEN_PATH}/amalgamate.py"
							"${OSQUERY_TABLES_PATH}/specs/blacklist")

LIST(APPEND GENERATION_DEPENDENCIES ${TABLE_FILES_TEMPLATES})

FOREACH(TABLE_FILE ${TABLE_FILES})
	SET(TABLE_FILE_GEN ${TABLE_FILE})
	STRING(REPLACE "${OSQUERY_TABLES_PATH}/specs"
				   "${OSQUERY_GENERATED_PATH}/tables"
				   TABLE_FILE_GEN
				   ${TABLE_FILE_GEN})
	STRING(REPLACE "linux/" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE "x/" "" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	STRING(REPLACE ".table" ".cpp" TABLE_FILE_GEN ${TABLE_FILE_GEN})
	ADD_CUSTOM_COMMAND(
		OUTPUT ${TABLE_FILE_GEN}
		COMMAND
			python "${OSQUERY_CODEGEN_PATH}/gentable.py" "${TABLE_FILE}" "${TABLE_FILE_GEN}" "$ENV{DISABLE_BLACKLIST}"
		DEPENDS
			${TABLE_FILE} ${GENERATION_DEPENDENCIES}

		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	LIST(APPEND GENERATED_TABLES ${TABLE_FILE_GEN})
ENDFOREACH()

SET(AMALGAMATION_FILE_GEN "${OSQUERY_GENERATED_PATH}/amalgamation.cpp")
ADD_CUSTOM_COMMAND(
	OUTPUT ${AMALGAMATION_FILE_GEN}
	COMMAND
		python "${OSQUERY_CODEGEN_PATH}/amalgamate.py" "${OSQUERY_TABLES_PATH}" "${OSQUERY_GENERATED_PATH}"
	DEPENDS
		${GENERATED_TABLES}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

## Library generation ###########################################################
# TODO(sangwan.kwon): Change amalgation files to additional
ADD_LIBRARY(osquery_generated_tables OBJECT "${AMALGAMATION_FILE_GEN}")
ADD_LIBRARY(osquery_static STATIC $<TARGET_OBJECTS:osquery_generated_tables>
								  $<TARGET_OBJECTS:osquery_sqlite>
								  ${${TARGET_OSQUERY_LIB}_SRCS}
								  ${${TARGET_OSQUERY_LIB_ADDITIONAL}_SRCS})
SET_TARGET_PROPERTIES(osquery_static PROPERTIES OUTPUT_NAME osquery_static)

# static_lib should include every object file in the archive in the link
ADD_LIBRARY(${TARGET_OSQUERY_LIB} STATIC main/lib.cpp)
TARGET_OSQUERY_LINK_WHOLE(${TARGET_OSQUERY_LIB} osquery_static)
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} ${${TARGET_OSQUERY_LIB}_DEP})

#INSTALL(TARGETS ${TARGET_OSQUERY_LIB}
#		DESTINATION ${CMAKE_INSTALL_LIBDIR})
#INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
#		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

## osqueryi generation ##########################################################
ADD_EXECUTABLE(${TARGET_OSQUERY_SHELL} devtools/shell.cpp main/shell.cpp)
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_SHELL} ${TARGET_OSQUERY_LIB})
INSTALL(TARGETS ${TARGET_OSQUERY_SHELL}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)

## osqueryd generation ##########################################################
ADD_EXECUTABLE(${TARGET_OSQUERY_DAEMON} main/daemon.cpp)
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_DAEMON} ${TARGET_OSQUERY_LIB})
INSTALL(TARGETS ${TARGET_OSQUERY_DAEMON}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)

## example extension with the SDK ##############################################
ADD_EXECUTABLE(example_extension examples/example_extension.cpp)
TARGET_LINK_LIBRARIES(example_extension ${TARGET_OSQUERY_LIB})
SET_TARGET_PROPERTIES(example_extension PROPERTIES OUTPUT_NAME example_extension.ext)

# Build the example extension module with the SDK
ADD_OSQUERY_MODULE(modexample examples/example_module.cpp)

