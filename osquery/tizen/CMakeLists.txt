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

ADD_OSQUERY_LIBRARY(osquery_tizen property/property.cpp
								  manager/manager.cpp
								  manager/manager_impl.cpp
								  notification/notification.cpp)

ADD_OSQUERY_TEST(${OSQUERY_TIZEN_TESTS})

IF(DEFINED GBS_BUILD)
	# tables
	FILE(GLOB TIZEN_TABLES "tables/*.cpp")
	ADD_OSQUERY_LIBRARY(tizen_tables ${TIZEN_TABLES})
	FILE(GLOB OSQUERY_TIZEN_TESTS "[!d]*/tests/*.cpp")

# Verification can be done with full-DPM
#	FILE(GLOB OSQUERY_GBS_TESTS "device_policy/tests/*.cpp")
#	ADD_OSQUERY_TEST(${OSQUERY_GBS_TESTS})
ENDIF(DEFINED GBS_BUILD)
