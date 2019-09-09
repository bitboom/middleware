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

SET(TARGET_OSQUERY_TEST osquery-test)
SET(TARGET_OSQUERY_SHELL osqueryi)

SET(${TARGET_OSQUERY_LIB}_SRCS "")
SET(${TARGET_OSQUERY_LIB}_DEPS "")
SET(${TARGET_OSQUERY_LIB}_TESTS "")

ADD_OSQUERY_LINK(glog
				 gflags
				 pthread
				 libthrift.a
				#rocksdb deps
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
				#shell deps
				 readline
				#build-in tables deps
				 systemd)

IF(DEFINED GBS_BUILD)
	SET(GBS_ONLY_PACKAGES klay
						  dpm-pil
						  vconf)

	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GBS_DEPS REQUIRED ${GBS_ONLY_PACKAGES})
	INCLUDE_DIRECTORIES(SYSTEM ${GBS_DEPS_INCLUDE_DIRS})

	ADD_OSQUERY_LINK(${GBS_DEPS_LIBRARIES})
ENDIF(DEFINED GBS_BUILD)

## osquery v1.5.0
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

## osquery v4.0.0
ADD_SUBDIRECTORY(utils)

ADD_LIBRARY(${TARGET_OSQUERY_LIB}
				STATIC main/lib.cpp
					   $<TARGET_OBJECTS:osquery_generated_tables>
					   $<TARGET_OBJECTS:osquery_sqlite>
					   ${${TARGET_OSQUERY_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_LIB} ${${TARGET_OSQUERY_LIB}_DEPS})
SET_TARGET_PROPERTIES(${TARGET_OSQUERY_LIB} PROPERTIES OUTPUT_NAME ${TARGET_OSQUERY_LIB})


ADD_EXECUTABLE(${TARGET_OSQUERY_TEST} main/tests.cpp
									  ${${TARGET_OSQUERY_LIB}_TESTS})
TARGET_LINK_WHOLE(${TARGET_OSQUERY_TEST} ${TARGET_OSQUERY_LIB})
TARGET_LINK_LIBRARIES(${TARGET_OSQUERY_TEST} gtest)
SET_TARGET_PROPERTIES(${TARGET_OSQUERY_TEST}
					  PROPERTIES COMPILE_FLAGS "-DGTEST_HAS_TR1_TUPLE=0")
ADD_TEST(${TARGET_OSQUERY_TEST} ${TARGET_OSQUERY_TEST})
INSTALL(TARGETS ${TARGET_OSQUERY_TEST}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)

ADD_EXECUTABLE(${TARGET_OSQUERY_SHELL} devtools/shell.cpp main/shell.cpp)
TARGET_LINK_WHOLE(${TARGET_OSQUERY_SHELL} ${TARGET_OSQUERY_LIB})
INSTALL(TARGETS ${TARGET_OSQUERY_SHELL}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
