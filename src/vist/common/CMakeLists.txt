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

PKG_CHECK_MODULES(VIST_COMMON_DEPS REQUIRED gflags klay dlog)

INCLUDE_DIRECTORIES(SYSTEM . ${VIST_COMMON_DEPS_INCLUDE_DIRS})

ADD_VIST_COMMON_LIBRARY(vist_common common.cpp)

FILE(GLOB COMMON_TESTS "tests/*.cpp")
ADD_VIST_TEST(${COMMON_TESTS})

ADD_LIBRARY(${TARGET_VIST_COMMON_LIB} STATIC ${${TARGET_VIST_COMMON_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_VIST_COMMON_LIB} ${VIST_COMMON_DEPS_LIBRARIES}
												pthread)
