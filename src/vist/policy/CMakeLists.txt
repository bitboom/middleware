# Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
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

SET(${TARGET_VIST_POLICY_LIB}_SRCS "")

ADD_VIST_POLICY_LIBRARY(vist_policy_core api.cpp
										 policy-manager.cpp
										 policy-loader.cpp
										 policy-storage.cpp)

FILE(GLOB POLICY_CORE_TESTS "tests/*.cpp")
ADD_VIST_TEST(${POLICY_CORE_TESTS})

ADD_LIBRARY(${TARGET_VIST_POLICY_LIB} STATIC ${${TARGET_VIST_POLICY_LIB}_SRCS})
TARGET_LINK_LIBRARIES(${TARGET_VIST_POLICY_LIB} ${TARGET_VIST_COMMON_LIB}
												vist-rmi
												pthread
												dl)
