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

FIND_PROGRAM(THRIFT_COMPILER thrift /usr/local/bin
									/usr/bin
									NO_DEFAULT_PATH)

# Generate the thrift intermediate/interface code.
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")
ADD_CUSTOM_COMMAND(
	COMMAND
		${THRIFT_COMPILER} --gen cpp:dense "${CMAKE_SOURCE_DIR}/osquery.thrift"
	DEPENDS
		"${CMAKE_SOURCE_DIR}/osquery.thrift"
	WORKING_DIRECTORY
		"${CMAKE_BINARY_DIR}/generated"
	OUTPUT
		${OSQUERY_THRIFT_GENERATED_FILES})

ADD_OSQUERY_LIBRARY(osquery_extensions ${OSQUERY_THRIFT_GENERATED_FILES}
									   extensions.cpp
									   interface.cpp)

FILE(GLOB OSQUERY_EXTENSIONS_TESTS "tests/*.cpp")

# TODO: Resolve failed cases
#ADD_OSQUERY_TEST(${OSQUERY_EXTENSIONS_TESTS})
