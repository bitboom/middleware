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

ADD_OSQUERY_LIBRARY(osquery_utils base64.cpp
								  chars.cpp
								  only_movable.cpp
								  status/status.cpp
								  system/time.cpp
								  system/posix/env.cpp
								  system/posix/errno.cpp
								  system/posix/filepath.cpp
								  system/posix/time.cpp
								  info/platform_type.cpp
								  config/default_paths.cpp
								  conversions/tryto.cpp
								  conversions/split.cpp
								  json/json.cpp)

FILE(GLOB OSQUERY_UTILS_ERROR_TESTS "error/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_ERROR_TESTS})

FILE(GLOB OSQUERY_UTILS_DEBUG_TESTS "debug/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_DEBUG_TESTS})

FILE(GLOB OSQUERY_UTILS_EXPECTED_TESTS "expected/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_EXPECTED_TESTS})

FILE(GLOB OSQUERY_UTILS_STATUS_TESTS "status/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_STATUS_TESTS})

FILE(GLOB OSQUERY_UTILS_CONVERSION_TESTS "conversions/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_CONVERSION_TESTS})

FILE(GLOB OSQUERY_UTILS_JSON_TESTS "json/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_JSON_TESTS})

FILE(GLOB OSQUERY_UTILS_SYSTEM_TESTS "system/tests/*.cpp")
FILE(GLOB OSQUERY_UTILS_SYSTEM_POSIXE_TESTS "system/posix/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_UTILS_SYSTEM_TESTS}
				 ${OSQUERY_UTILS_SYSTEM_POSIXE_TESTS})
