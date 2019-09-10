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

ADD_OSQUERY_LINK(ssl
				 z)

ADD_OSQUERY_LIBRARY(osquery_remote http_client.cpp
								   requests.cpp
								   uri.cpp
								   enroll/enroll.cpp
								   enroll/tls_enroll.cpp
								   serializers/json.cpp
								   transports/tls.cpp)

FILE(GLOB OSQUERY_REMOTE_TESTS "tests/*.cpp")
FILE(GLOB OSQUERY_REMOTE_EXTRA_TESTS "*/tests/*.cpp")
ADD_OSQUERY_TEST(${OSQUERY_REMOTE_TESTS}
				 ${OSQUERY_REMOTE_EXTRA_TESTS})
