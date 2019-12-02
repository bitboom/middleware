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

SET(${TARGET_VIST_CLIENT_LIB}_SRCS "")

ADD_VIST_CLIENT_LIBRARY(vist_client query.cpp
									virtual-table.cpp)

FILE(GLOB CLIENT_TESTS "tests/*.cpp")
IF(DEFINED GBS_BUILD)
	ADD_VIST_TEST(${CLIENT_TESTS})
ENDIF(DEFINED GBS_BUILD)

ADD_LIBRARY(${TARGET_VIST_CLIENT_LIB} STATIC ${${TARGET_VIST_CLIENT_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_VIST_CLIENT_LIB} ${VIST_CLIENT_DEPS_LIBRARIES}
												${TARGET_VIST_COMMON_LIB}
												pthread)
