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

SET(TARGET_VIST_DAEMON vistd)
SET(TARGET_VIST_TEST vist-test)

SET(${TARGET_VIST_LIB}_SRCS "")
SET(${TARGET_VIST_LIB}_DEPS "")
SET(${TARGET_VIST_LIB}_TESTS "")

INCLUDE_DIRECTORIES(. common)

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(manager)
ADD_SUBDIRECTORY(notification)
ADD_SUBDIRECTORY(property)
ADD_SUBDIRECTORY(service)

ADD_LIBRARY(${TARGET_VIST_LIB} STATIC ${${TARGET_VIST_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_VIST_LIB} ${${TARGET_VIST_LIB}_DEPS}
										 ${TARGET_OSQUERY_LIB})

ADD_EXECUTABLE(${TARGET_VIST_DAEMON} main/main.cpp)
TARGET_LINK_LIBRARIES(${TARGET_VIST_DAEMON} ${TARGET_VIST_LIB})
TARGET_LINK_WHOLE(${TARGET_VIST_DAEMON} ${TARGET_OSQUERY_LIB})
INSTALL(TARGETS ${TARGET_VIST_DAEMON}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)

ADD_EXECUTABLE(${TARGET_VIST_TEST} main/tests.cpp
								   ${${TARGET_VIST_LIB}_TESTS})
TARGET_LINK_LIBRARIES(${TARGET_VIST_TEST} ${TARGET_VIST_LIB}
										   gtest)
TARGET_LINK_WHOLE(${TARGET_VIST_TEST} ${TARGET_OSQUERY_LIB})
ADD_TEST(${TARGET_VIST_TEST} ${TARGET_VIST_TEST})
INSTALL(TARGETS ${TARGET_VIST_TEST}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)
