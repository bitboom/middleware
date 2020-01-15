#  Copyright (c) 2019-present Samsung Electronics Co., Ltd All Rights Reserved
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

SET(TARGET vist-rmi)
SET(${TARGET}_SRCS gateway.cpp
				   remote.cpp
				   message.cpp
				   impl/socket.cpp
				   impl/eventfd.cpp
				   impl/mainloop.cpp
				   impl/connection.cpp)

ADD_LIBRARY(${TARGET} SHARED ${${TARGET}_SRCS})
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
TARGET_LINK_LIBRARIES(${TARGET} ${TARGET_VIST_COMMON_LIB}
								pthread)
INSTALL(TARGETS ${TARGET}
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ
					OWNER_WRITE
					OWNER_EXECUTE
					GROUP_READ
					GROUP_EXECUTE
					WORLD_READ
					WORLD_EXECUTE)

ADD_LIBRARY(${TARGET}-static STATIC ${${TARGET}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET} ${TARGET_VIST_COMMON_LIB}
								pthread)

FILE(GLOB RMI_TESTS "tests/*.cpp")
ADD_VIST_TEST(${RMI_TESTS})

FILE(GLOB RMI_IMPL_TESTS "impl/tests/*.cpp")
ADD_VIST_TEST(${RMI_IMPL_TESTS})
